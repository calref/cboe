
#include <math.h>

//#include "item.h"

#include "boe.global.h"
#include "boe.actions.h"
#include "boe.graphutil.h"
#include "boe.graphics.h"
#include "boe.townspec.h"
#include "boe.fileio.h"
#include "boe.dlgutil.h"
#include "boe.locutils.h"
#include "boe.fields.h"
#include "boe.town.h"
#include "boe.text.h"
#include "boe.party.h"
#include "boe.monster.h"
#include "boe.specials.h"
#include "boe.newgraph.h"
#include "boe.combat.h"
#include "boe.items.h"
#include "soundtool.h"
#include "boe.infodlg.h"
#include "boe.itemdata.h"
#include "boe.main.h"
#include "mathutil.h"


Rect bottom_buttons[7];
Rect town_buttons[10];
Rect combat_buttons[9];
Rect world_screen = {23, 23, 346, 274};
Rect item_screen_button_rects[9] = {{126,11,140,28},{126,40,140,57},{126,69,140,86},{126,98,140,115},
{126,127,140,144},{126,156,140,173},{126,176,141,211},
{126,213,125,248},//// made rect disappear
{127,251,139,267}};
	
Rect border_rect[4] = {{5, 5, 15, 283}, {5, 5, 355, 15},
						{345, 5, 355, 283}, {5, 273, 355, 283}};
Rect medium_buttons[4] = {{383,190,401,225}, {402, 190, 420, 225},
							{383, 227, 401, 263}, {402, 227,420, 263}}; ; 

Rect item_buttons[8][6];
 // name, use, give, drip, info, sell/id
Rect pc_buttons[6][5];
 // name, hp, sp, info, trade
short num_chirps_played = 0;

extern Rect startup_button[6];
Rect startup_top;

// For menu spell casting, some info needs to be stored up here.
short refer_mage[62] = {0,2,1,1,2,2,0,2,2,0, 2,2,2,2,1,2,2,2,2,2, 0,1,2,0,2,2,3,3,2,1, 
							2,2,1,0,2,2,3,2, 0,1,2,0,2,3,2,3, 2,1,2,3,2,2,2,0, 1,1,1,0,3,2,2,3};
short refer_priest[62] = {1,0,0,2,0,0,0,0,0,2, 1,0,2,0,2,2,0,2,3,0, 0,0,2,0,0,0,2,0,0,3, 
							0,1,2,0,3,0,0,0, 1,0,0,2,0,3,0,2, 0,0,0,0,2,1,1,1, 0,2,0,2,1,2,0,0};
	// 0 - refer  1 - do in combat immed.  2 - need targeting  3 - need fancy targeting
short mage_need_select[62] = {0,0,1,1,0,0,0,0,0,0, 0,0,0,0,1,0,0,0,0,0, 0,1,0,0,0,0,0,0,0,1,
						0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,1,0,0,0,0};
short priest_need_select[62] = {1,1,1,0,0,1,1,0,0,0, 1,1,0,0,0,0,0,0,0,1, 1,0,0,0,1,0,0,1,1,0,
						0,0,0,1,0,1,1,0, 0,1,2,0,0,0,0,0, 0,1,0,2,0,0,0,0, 0,0,2,0,0,0,0,0};
// 0 - no select  1 - active only  2 - any existing

word_rect_type preset_words[9] = {{"Look",{366,4,386,54}},{"Name",{366,70,386,130}},{"Job",{366,136,386,186}},
								{"Buy",{389,4,409,54}},{"Sell",{389,70,409,120}},{"Record",{389,121,409,186}},
								{"Done",{389,210,409,270}},{"Go Back",{366,190,386,270}},
								{"Ask About...",{343,4,363,134}}};
cItemRec start_items[6] = {cItemRec('nife'),cItemRec('buck'),cItemRec('bow '),cItemRec('arrw'),cItemRec('pole'),cItemRec('helm')};
Boolean item_area_button_active[8][6];
Boolean pc_area_button_active[6][5];
short item_bottom_button_active[9] = {0,0,0,0,0, 0,1,1,1};
Boolean unreg_party_in_scen_not_check = FALSE;

Rect pc_help_button,pc_area_rect,item_area_rect;

short current_terrain_type = 0,num_out_moves = 0;
short door_pc,store_drop_item;
short current_switch = 6;
out_wandering_type store_wandering_special;
long dummy;
short store_shop_type;

short debug_ok = 0;
short store_selling_values[8] = {0,0,0,0,0,0,0,0};

extern short cen_x, cen_y, overall_mode, stat_window,give_delays,pc_moves[6];
extern Point	to_create;
extern Boolean in_startup_mode,All_Done,play_sounds,frills_on,spell_forced,save_maps,monsters_going;
extern Boolean debug_on,registered,cartoon_happening,party_in_memory,in_scen_debug;

// game info globals

extern WindowPtr	mainPtr;	
extern party_record_type	party;
extern pc_record_type adven[6];
extern outdoor_record_type outdoors[2][2];
extern current_town_type	c_town;
extern big_tr_type t_d;
extern unsigned char out[96][96],out_e[96][96];
extern short which_item_page[6];
extern short town_size[3],store_spell_target,pc_last_cast[2][6],pc_casting,store_mage,store_priest;
extern town_item_list	t_i; // shouldn't be here
extern unsigned char misc_i[64][64];
extern short spec_item_array[60];
extern cScenario scenario;
extern piles_of_stuff_dumping_type *data_store;

// combat globals


extern short which_combat_type,num_targets_left;
extern unsigned char combat_terrain[64][64];
extern location center;
extern location pc_pos[6];
extern short current_pc,town_type;
extern short combat_active_pc,stat_screen_mode;

extern Boolean modeless_exists[18],diff_depth_ok;
extern short modeless_key[18];
extern DialogPtr modeless_dialogs[18];

extern stored_items_list_type stored_items[3];
extern stored_outdoor_maps_type o_maps;
extern Point ul;
extern ControlHandle text_sbar,item_sbar,shop_sbar;
extern short shop_identify_cost;
extern KeyMap key_state;
extern Boolean fry_startup;


char *dir_string[] = {"North", "NorthEast", "East", "SouthEast", "South", "SouthWest", "West", "NorthWest"};
char get_new_terrain();
creature_start_type	save_monster_type;

short wand_loc_count = 0;
short monst_place_count = 0; // 1 - standard place	2 - place last

// 0 - whole area, 1 - active area 2 - graphic 3 - item name
// 4 - item cost 5 - item extra str  6 - item help button
Rect shopping_rects[8][7];
short special_queue[20];
Boolean end_scenario = FALSE;

void init_screen_locs() ////
{
	short i,j,k,l;
	Rect startup_base = {279,5,327,306};
	Rect shop_base = {63,12,99,267};
	
	for (i = 0; i < 7; i++)
		shopping_rects[0][i] = shop_base;
	shopping_rects[0][SHOPRECT_ACTIVE_AREA].right -= 35;
	shopping_rects[0][SHOPRECT_GRAPHIC].right = shopping_rects[0][SHOPRECT_GRAPHIC].left + 28;
	shopping_rects[0][SHOPRECT_ITEM_NAME].top += 4;
	shopping_rects[0][SHOPRECT_ITEM_NAME].left += 28;
	shopping_rects[0][SHOPRECT_ITEM_COST].top += 20;
	shopping_rects[0][SHOPRECT_ITEM_COST].left += 154;
	shopping_rects[0][SHOPRECT_ITEM_EXTRA].top += 20;
	shopping_rects[0][SHOPRECT_ITEM_EXTRA].left += 34;
	shopping_rects[0][SHOPRECT_ITEM_HELP].top += 3;
	shopping_rects[0][SHOPRECT_ITEM_HELP].bottom -= 21;
	shopping_rects[0][SHOPRECT_ITEM_HELP].right -= 19;
	shopping_rects[0][SHOPRECT_ITEM_HELP].left = shopping_rects[0][SHOPRECT_ITEM_HELP].right - 14;
	for (i = 1; i < 8; i++)
		for (j = 0; j < 7; j++) {
			shopping_rects[i][j] = shopping_rects[0][j];
			OffsetRect(&shopping_rects[i][j],0,i * 36);
			}
	

	for (i = 0; i < 6; i++) {
		startup_button[i] = startup_base;
		OffsetRect(&startup_button[i],301 * (i / 3)  - 18,48 * (i % 3));
		}
	startup_top.top = 5;
	startup_top.bottom = startup_button[STARTBTN_LOAD].top;
	startup_top.left = 5;
	startup_top.right = startup_button[STARTBTN_JOIN].right;
		
	for (i = 0; i < 200; i++)
		for (j = 0; j < 8; j++)
			for (k = 0; k < 64; k++)////
				data_store->town_maps.town_maps[i][j][k] = 0;

	for (i = 0; i < 100; i++)
			for (k = 0; k < 6; k++)
				for (l = 0; l < 48; l++)
					o_maps.outdoor_maps[i][k][l] = 0;

	for (i = 0; i < 7; i++) {
		bottom_buttons[i].top = 383;
		bottom_buttons[i].bottom = 420;
		bottom_buttons[i].left = 5 + (i * 37);
		bottom_buttons[i].right = bottom_buttons[i].left + 36;
		town_buttons[i] = bottom_buttons[i];

		//FrameRect (&bottom_buttons[i]);
		}

	for (i = 0; i < 5; i++) {
		combat_buttons[i] = bottom_buttons[i];
		}
	town_buttons[7] = bottom_buttons[6];
	town_buttons[5] = medium_buttons[0];
	town_buttons[6] = medium_buttons[1];
	for (i = 5; i < 9; i++) {
		combat_buttons[i] = medium_buttons[i - 5];
		}
	
 // name, use, give, drip, info, sell/id   each one 13 down
	item_buttons[0][ITEMBTN_NAME].top = 17;
	item_buttons[0][ITEMBTN_NAME].bottom = item_buttons[0][ITEMBTN_NAME].top + 12;
	item_buttons[0][ITEMBTN_NAME].left = 3;
	item_buttons[0][ITEMBTN_NAME].right = item_buttons[0][ITEMBTN_NAME].left + 185;
	item_buttons[0][ITEMBTN_USE] = item_buttons[0][0];
	item_buttons[0][ITEMBTN_USE].left = 196;
	item_buttons[0][ITEMBTN_USE].right = 210;
	item_buttons[0][ITEMBTN_GIVE] = item_buttons[0][ITEMBTN_NAME];
	item_buttons[0][ITEMBTN_GIVE].left = 210;
	item_buttons[0][ITEMBTN_GIVE].right = 224;
	item_buttons[0][ITEMBTN_DROP] = item_buttons[0][ITEMBTN_NAME];
	item_buttons[0][ITEMBTN_DROP].left = 224;
	item_buttons[0][ITEMBTN_DROP].right = 238;
	item_buttons[0][ITEMBTN_INFO] = item_buttons[0][ITEMBTN_NAME];
	item_buttons[0][ITEMBTN_INFO].left = 238;
	item_buttons[0][ITEMBTN_INFO].right = 252;
	item_buttons[0][ITEMBTN_SPEC] = item_buttons[0][ITEMBTN_NAME];
	item_buttons[0][ITEMBTN_SPEC].left = 173;
	item_buttons[0][ITEMBTN_SPEC].right = 232;
	for (i = 1; i < 8; i++) 
		for (j = 0; j < 6; j++) {  
		item_buttons[i][j] = item_buttons[0][j];
		OffsetRect(&item_buttons[i][j],0,13 * i);
		}
	
/*	for (i = 0; i < 8; i++) {
		item_screen_button_rects[i] = bottom_base;
		OffsetRect(&item_screen_button_rects[i],10 + i * 29,126);
		}
	item_screen_button_rects[6].left = 176;
	item_screen_button_rects[6].right = 211;
	item_screen_button_rects[7].left = 213;
	item_screen_button_rects[7].right = 248;
	item_screen_button_rects[8].top = 127;
	item_screen_button_rects[8].bottom = 140;
	item_screen_button_rects[8].left = 251;
	item_screen_button_rects[8].right = 267; */

 // name, hp, sp, info, trade
	pc_buttons[0][PCBTN_NAME].top = 18;
	pc_buttons[0][PCBTN_NAME].bottom = pc_buttons[0][PCBTN_NAME].top + 12;
	pc_buttons[0][PCBTN_NAME].left = 3;
	pc_buttons[0][PCBTN_NAME].right = pc_buttons[0][PCBTN_NAME].left + 177;
	pc_buttons[0][PCBTN_HP] = pc_buttons[0][PCBTN_NAME];
	pc_buttons[0][PCBTN_HP].left = 184;
	pc_buttons[0][PCBTN_HP].right = 214;
	pc_buttons[0][PCBTN_SP] = pc_buttons[0][PCBTN_NAME];
	pc_buttons[0][PCBTN_SP].left = 214;
	pc_buttons[0][PCBTN_SP].right = 237;
	pc_buttons[0][PCBTN_INFO] = pc_buttons[0][PCBTN_NAME];
	pc_buttons[0][PCBTN_INFO].left = 241;
	pc_buttons[0][PCBTN_INFO].right = 253;
	pc_buttons[0][PCBTN_TRADE] = pc_buttons[0][PCBTN_NAME];
	pc_buttons[0][PCBTN_TRADE].left = 253;
	pc_buttons[0][PCBTN_TRADE].right = 262;
	for (i = 1; i < 6; i++) 
		for (j = 0; j < 5; j++) { 
		pc_buttons[i][j] = pc_buttons[0][j];
		OffsetRect(&pc_buttons[i][j],0,13 * i);
		}
	pc_help_button.top = 101;
	pc_help_button.bottom = 114;
	pc_help_button.left = 251;
	pc_help_button.right = 267;
	
	pc_area_rect.top = PC_WIN_UL_Y;
	pc_area_rect.left = PC_WIN_UL_X;
	pc_area_rect.bottom = PC_WIN_UL_Y + 116;
	pc_area_rect.right = PC_WIN_UL_X + 271;
	item_area_rect.top = ITEM_WIN_UL_Y;
	item_area_rect.left = ITEM_WIN_UL_X;
	item_area_rect.bottom = ITEM_WIN_UL_Y + 143;
	item_area_rect.right = ITEM_WIN_UL_X + 271;
}

Boolean prime_time()
{
	if ((overall_mode < MODE_TALK_TOWN) || (overall_mode == MODE_COMBAT))
		return TRUE;
	return FALSE;
}

Boolean handle_action(EventRecord event)
{
	short i,j,k,item_hit,which_t,s1,s2,s3;
	Boolean are_done = FALSE;
	Boolean need_redraw = FALSE, did_something = FALSE, need_reprint = FALSE;
	Boolean town_move_done = FALSE,pc_delayed = FALSE,storage;
	location destination,cur_loc,loc_in_sec,cur_direction = {0,0};
	unsigned char debug_storage;
	short find_direction_from,ter_looked_at,button_hit = 12,store_cur_pc;
	short store_sp[6];
	unsigned char ter;
	
	char str[60];	
	Point the_point,point_in_area;

	
	debug_storage = party.spec_items[1];
	
	the_point = event.where;
	the_point.h -= ul.h;
	the_point.v -= ul.v;
	for (i = 0; i < 20; i++)
		special_queue[i] = -1;
	end_scenario = FALSE;
		
	if (unreg_party_in_scen_not_check == TRUE) {
		if (enter_password() == FALSE) {
			end_scenario = TRUE;
			}
			else {
				in_scen_debug = TRUE;
				ASB("Debug mode ON.");
				}
		unreg_party_in_scen_not_check = FALSE;
		}
	// Now split off the extra stuff, like talking and shopping.
	if (overall_mode == MODE_TALKING) {
		handle_talk_event(the_point);
		if (overall_mode != MODE_TALKING)
			return FALSE;
		}
	if (overall_mode == MODE_SHOPPING) {
		handle_shop_event(the_point);
		if (overall_mode != MODE_SHOPPING)
			return FALSE;
		}
		
	num_chirps_played = 0;

// First, figure out where party is
	switch (overall_mode) {
		case MODE_OUTDOORS: case MODE_LOOK_OUTDOORS:
			cur_loc = party.p_loc;
			for (i = 0; i < 7; i++)
				if (PtInRect (the_point, &bottom_buttons[i]) == TRUE) {
					button_hit = i;
					if (spell_forced == FALSE)
						main_button_click(overall_mode,bottom_buttons[i]);					
					}
			break;
			
		case MODE_TOWN: case MODE_TALK_TOWN: case MODE_USE: case MODE_LOOK_TOWN: case 5:
//			cur_loc = c_town.p_loc;
			cur_loc = center;
			for (i = 0; i < 8; i++)
				if (PtInRect (the_point, &town_buttons[i]) == TRUE) {
					button_hit = i;
					if (spell_forced == FALSE)
					main_button_click(overall_mode,town_buttons[i]);					
					}
			break;
			
		case MODE_TALKING: case MODE_SHOPPING: break;
		
		case MODE_TOWN_TARGET: case MODE_COMBAT: case MODE_SPELL_TARGET: case MODE_FIRING: case MODE_THROWING:
		case MODE_FANCY_TARGET: case MODE_DROPPING: case MODE_LOOK_COMBAT:
			cur_loc = (overall_mode > MODE_COMBAT) ? center : pc_pos[current_pc];
			for (i = 0; i < 9; i++)
				if (PtInRect (the_point, &combat_buttons[i]) == TRUE) {
					button_hit = i;
					if (spell_forced == FALSE)
					main_button_click(overall_mode,combat_buttons[i]);					
					}
			break;
		}

// Then, handle a button being hit.
	if (button_hit != 12)
			switch(button_hit)
			{
				case 0: case 1:
					if (someone_awake() == FALSE) {
						ASB("Everyone's asleep/paralyzed.");
						need_reprint = TRUE;
						need_redraw = TRUE;						
						}
					if (overall_mode == MODE_OUTDOORS) {
						cast_spell(button_hit,0);
						spell_forced = FALSE;
						need_reprint = TRUE;
						need_redraw = TRUE;
						}
					else if (overall_mode == MODE_TOWN) {
						for (i = 0; i < 6; i++)
							store_sp[i] = adven[i].cur_sp;
						cast_spell(button_hit,1);
						spell_forced = FALSE;
						need_reprint = TRUE;
						need_redraw = TRUE;
						for (i = 0; i < 6; i++)
							if (store_sp[i] != adven[i].cur_sp)
								did_something = TRUE;
						}
						else if (overall_mode == MODE_TOWN_TARGET) {
							add_string_to_buf("  Cancelled.                   ");
							overall_mode = MODE_TOWN;
							}
					else if (overall_mode > MODE_COMBAT) {
						if ((overall_mode == MODE_COMBAT) && (button_hit == 0)) {
							did_something = combat_cast_mage_spell();
							need_reprint = TRUE;
						}
						if ((overall_mode == MODE_COMBAT) && (button_hit == 1)) {
							did_something = combat_cast_priest_spell();
							need_reprint = TRUE;
						}
						if ((overall_mode != MODE_SPELL_TARGET) && (overall_mode != MODE_FANCY_TARGET)) {
							need_redraw = TRUE;
						}
						else did_something = FALSE;
						spell_forced = FALSE;
						redraw_terrain();
						}
					else if ((overall_mode == MODE_SPELL_TARGET) || (overall_mode == MODE_FANCY_TARGET)) {
						add_string_to_buf("  Cancelled.         ");
						overall_mode = MODE_COMBAT;
						center = pc_pos[current_pc];
						pause(10);
						need_redraw = TRUE;						
						}
					put_pc_screen();
					put_item_screen(stat_window,0);
				break;

				case 2:
					if (overall_mode == MODE_OUTDOORS) {
						overall_mode = MODE_LOOK_OUTDOORS;
						add_string_to_buf("Look: Select a space. Option-click        ");
						add_string_to_buf("  to look more than once.");
						need_redraw = TRUE;
						}
					if (overall_mode == MODE_TOWN) {
						overall_mode = MODE_LOOK_TOWN;
						add_string_to_buf("Look: Select a space. Option-click        ");
						add_string_to_buf("  to look more than once.");						
						need_redraw = TRUE;
						}					
					if (overall_mode == MODE_COMBAT) {
						overall_mode = MODE_LOOK_COMBAT;
						add_string_to_buf("Look: Select a space. Option-click        ");
						add_string_to_buf("  to look more than once.");
						need_redraw = TRUE;
						}						
					break;

				case 3:
					if (overall_mode == MODE_COMBAT) {
						add_string_to_buf("Parry. ");
						char_parry();
						did_something = TRUE;
						need_reprint = TRUE;
						need_redraw = TRUE;
						}
					if (overall_mode == MODE_TOWN) {
						overall_mode = MODE_TALK_TOWN;
						add_string_to_buf("Talk: Select someone    ");
						need_reprint = TRUE;
						}
					if (overall_mode == MODE_OUTDOORS) { // Resting
						i = 0;
						ter = out[party.p_loc.x][party.p_loc.y];
						if (party.in_boat >= 0)////
							add_string_to_buf("Rest:  Not in boat.               ");
						else if (someone_poisoned() == TRUE)
							add_string_to_buf("Rest: Someone poisoned.           ");
							else if (party.food <= 12)
								add_string_to_buf("Rest: Not enough food.            ");
								else if (nearest_monster() <= 3)
								add_string_to_buf("Rest: Monster too close.            ");
								else if ((scenario.ter_types[ter].special >= 2) && (scenario.ter_types[ter].special <= 6))
									add_string_to_buf("Rest: It's dangerous here.");////
								else if (flying() == TRUE)
									add_string_to_buf("Rest: Not while flying.           ");
								else {
										add_string_to_buf("Resting...                    ");
										print_buf();
										play_sound(20);
										draw_rest_screen();
										pause(25);
										party.food -= 6;
										while (i < 50) {
											increase_age();
											j = get_ran(1,1,2);
											if (j == 2)
												do_monsters();
											j = get_ran(1,1,70);
											if (j == 10)
												create_wand_monst();
											if (nearest_monster() <= 3) {
												i = 200;
												add_string_to_buf("  Monsters nearby.");
												}
												else i++;
											}
										put_pc_screen();
									}
							if (i == 50) {
								party.age += 1200;////
								add_string_to_buf("  Rest successful.                ");
								k = get_ran(5,1,10);
								heal_party(k);
								restore_sp_party(50);
								put_pc_screen();
								}
							need_reprint = TRUE;
							need_redraw = TRUE;
						}
						break;

				case 4:
					if (overall_mode == MODE_OUTDOORS) {
						give_help(62,0,0);
						display_map();
						make_cursor_sword();
						}
					if ((overall_mode == MODE_TOWN) || (overall_mode == MODE_COMBAT)) {
						if (party.in_boat >= 0) {////
							j = 0;
							add_string_to_buf("Get: Not while in boat.           ");
							}
						if (overall_mode == MODE_TOWN)
							j = get_item(c_town.p_loc,6,FALSE);
						else {
							j = get_item(pc_pos[current_pc],current_pc,FALSE);
							take_ap(4);
						}
						if (j > 0) {
							put_item_screen(stat_window, 0);
							put_pc_screen();
							need_redraw = TRUE;
							did_something = TRUE;
							}
						need_reprint = TRUE;
						}
					break;

				case 5:
					if (overall_mode == MODE_OUTDOORS) {
						save_file(0);
						need_redraw = TRUE;
						current_switch = 6;
						break;
						}
					if (overall_mode == MODE_TOWN) {
						add_string_to_buf("Use: Select a space or item.");
						add_string_to_buf("  (Hit button again to cancel.)");
						need_reprint = TRUE;
						overall_mode = MODE_USE;				
					}
					else if (overall_mode == MODE_USE) {
						overall_mode = MODE_TOWN;
						need_reprint = TRUE;
						add_string_to_buf("  Cancelled.");						
					}
					if (overall_mode == MODE_COMBAT) {
						need_reprint = TRUE;
						need_redraw = TRUE;
						pc_delayed = TRUE;
					}
						
					break;

				case 6: 
					if (overall_mode == MODE_OUTDOORS) {
						do_load();
						break;
						}
					if (overall_mode == MODE_TOWN) {
						give_help(62,0,0);
						display_map();
						make_cursor_sword();
						}
					if (overall_mode == MODE_COMBAT) {
						load_missile();
						need_reprint = TRUE;
						redraw_terrain();
						}
					else if ((overall_mode == MODE_FIRING) || (overall_mode == MODE_THROWING)) {
						add_string_to_buf("  Cancelled.             ");
						center = pc_pos[current_pc];
						pause(10);
						need_redraw = TRUE;
						overall_mode = MODE_COMBAT;
					}
					break;

				case 7:
					if (overall_mode == MODE_TOWN) {
						if (party.in_boat >= 0) {////
							need_reprint = TRUE;
							add_string_to_buf("Combat: Not while in boat.           ");
						}
						else if (party.in_horse >= 0) {
							need_reprint = TRUE;
							add_string_to_buf("Combat: Not while on horseback.           ");
						}
						else {
							add_string_to_buf("Combat!                 ");
							play_sound(18);
							need_reprint = TRUE;
							start_town_combat(party.direction);
							need_redraw = TRUE;
							current_pc = 6;
							did_something = TRUE;
							put_pc_screen();
						}
					}
					else if (overall_mode == MODE_COMBAT) {
						if (which_combat_type == MODE_OUTDOORS) {
							if (hit_end_c_button() == TRUE) {
								end_town_mode(0,c_town.p_loc);
								play_sound(93);
								add_string_to_buf("End combat.               ");
								handle_wandering_specials(0,1);
								menu_activate(1);					
								put_pc_screen();
								set_stat_window(current_pc);
							}
							else add_string_to_buf("Can't end combat yet.        ");
						}
						else {
							party.direction = end_town_combat();
							center = c_town.p_loc;
							//put_pc_screen();
							set_stat_window(current_pc);
							redraw_screen();
							play_sound(93);
							need_reprint = TRUE;
							need_redraw = TRUE;
							did_something = TRUE;
							menu_activate(1);					
						}				
						need_redraw = TRUE;
						}
					break;
					
				case 8:
					if (overall_mode == MODE_COMBAT) {
						if (combat_active_pc == 6) {
							add_string_to_buf("This PC now active.           ");
							combat_active_pc = current_pc;
						}
						else {
							add_string_to_buf("All PC's now active.           ");
							current_pc = combat_active_pc;
							combat_active_pc = 6;							
						}
						need_reprint = TRUE;
					}
					break;
			}

/*	if (overall_mode == 30) /// I don't know what this is for.
		if (PtInRect(the_point, &text_panel_rect)) {
			through_sending();
			restore_mode();
			print_buf();
			} */
			
// Begin : click in terrain
	if ((PtInRect (the_point, &world_screen)) && ((is_out()) || (is_town()) || (is_combat())) ){		
		i = (the_point.h - 23) / 28;
		j = (the_point.v - 23) / 36;

		destination = cur_loc;

		if ((overall_mode == MODE_OUTDOORS) || (overall_mode == MODE_TOWN) || (overall_mode == MODE_COMBAT))
		if ((i == 4) & (j == 4)) { // Pausing
			if (overall_mode == MODE_COMBAT) {
				char_stand_ready();
				add_string_to_buf("Stand ready.  ");
				if (adven[current_pc].status[6] > 0) {
					add_string_to_buf("You clean webs.  ");
					adven[current_pc].status[6] = move_to_zero(adven[current_pc].status[6]);	
					adven[current_pc].status[6] = move_to_zero(adven[current_pc].status[6]);
					put_pc_screen();	
					}
				check_fields(pc_pos[current_pc],2,current_pc);
				}
				else {
					add_string_to_buf("Pause.");
					for (k = 0; k < 6; k++)
						if ((adven[k].main_status == 1) && (adven[k].status[6] > 0)) {
							sprintf((char *) str,"%s cleans webs.",adven[k].name);
							add_string_to_buf((char *) str);
							adven[k].status[6] = move_to_zero(adven[k].status[6]);
							adven[k].status[6] = move_to_zero(adven[k].status[6]);
							}
					if (party.in_horse >= 0) {
						if (overall_mode == MODE_OUTDOORS) {
							party.horses[party.in_horse].which_town = 200;
							party.horses[party.in_horse].loc_in_sec = global_to_local(party.p_loc);
							party.horses[party.in_horse].loc = party.p_loc;
							party.horses[party.in_horse].sector.x = party.outdoor_corner.x + party.i_w_c.x;
							party.horses[party.in_horse].sector.y = party.outdoor_corner.y + party.i_w_c.y;
							party.in_horse = -1;
							}
							else if (overall_mode == MODE_TOWN){
								party.horses[party.in_horse].loc = c_town.p_loc;
								party.horses[party.in_horse].which_town = c_town.town_num;
								party.in_horse = -1;
								}
						}
					put_pc_screen();	
					check_fields(c_town.p_loc,1,0);
					}

			did_something = TRUE;
			need_redraw = TRUE;
			}
		else {
			cur_direction = get_cur_direction(the_point);
			destination.x += cur_direction.x;
			destination.y += cur_direction.y;
				
	for (i = 0; i < 6; i++) // debug
		if (adven[i].exp_adj != 100) {
			ASB("Error: Flag 1");
			}

// Begin: Moving
				if (overall_mode == MODE_COMBAT) {
					if (pc_combat_move(destination) == TRUE) {
						center = pc_pos[current_pc];
						did_something = TRUE;
						update_explored(destination);
						}
					need_redraw = TRUE;
					menu_activate(1);
					}					
				if (overall_mode == MODE_TOWN) {
					if (someone_awake() == FALSE) {
						ASB("Everyone's asleep/paralyzed.");
						need_reprint = TRUE;
						need_redraw = TRUE;						
						}
						else {
						need_redraw = TRUE;
						if (town_move_party(destination,0) == TRUE) {
							did_something = TRUE;
							center = c_town.p_loc;
							update_explored(destination);

							if (loc_off_act_area(c_town.p_loc) == TRUE) {

								destination = end_town_mode(0,destination);


								town_move_done = TRUE;
								FlushEvents(mDownMask,0);				
								FlushEvents(keyDownMask,0);
								} 
							}
							else need_reprint = TRUE;
							menu_activate(1);
							}
					}										
				if (overall_mode == MODE_OUTDOORS)  {
					if (outd_move_party(destination,town_move_done) == TRUE) {
						center = destination;
						need_redraw = TRUE;
						did_something = TRUE;
						update_explored(party.p_loc);

						menu_activate(1);
					}
						else need_redraw = TRUE;					
					
					storage = out[party.p_loc.x][party.p_loc.y];
					if (scenario.ter_types[storage].special == 21) {//// town entry

						if (party.direction == 0) find_direction_from = 2;
						else if (party.direction == 4) find_direction_from = 0;
						else if (party.direction < 4) find_direction_from = 3;
						else find_direction_from = 1;
					
						for (i = 0; i < 8; i++) 
							if (party.loc_in_sec == outdoors[party.i_w_c.x][party.i_w_c.y].exit_locs[i]) {	
								which_t = outdoors[party.i_w_c.x][party.i_w_c.y].exit_dests[i];
								if (which_t >= 0)
									start_town_mode(outdoors[party.i_w_c.x][party.i_w_c.y].exit_dests[i], find_direction_from);
								if (is_town() == TRUE) {
									need_redraw = FALSE;
									i = 8;
									if (party.in_boat >= 0)
										party.boats[party.in_boat].which_town = c_town.town_num;
									if (party.in_horse >= 0)
										party.horses[party.in_horse].which_town = c_town.town_num;
									}
								}
					}
					} // End if(overall_mode == MODE_OUTDOORS)

				}	
// End: Moving

// Begin: Looking at something				
		if ((overall_mode == MODE_LOOK_OUTDOORS) || (overall_mode == MODE_LOOK_TOWN) || (overall_mode == MODE_LOOK_COMBAT)) {
			destination.x = destination.x + i - 4;
			destination.y = destination.y + j - 4;

			need_reprint = TRUE;
			
//			if ((can_see(cur_loc,destination) >= 4) || ((overall_mode != 35) && (loc_off_world(destination) == TRUE)))
				if ((overall_mode != MODE_LOOK_COMBAT) && (party_can_see(destination) == 6))
					add_string_to_buf("  Can't see space.         ");
				else if ((overall_mode == MODE_LOOK_COMBAT) && (can_see(pc_pos[current_pc],destination,0) >= 4))
					add_string_to_buf("  Can't see space.         ");
				else {
					add_string_to_buf("You see...               ");
					ter_looked_at = do_look(destination);
					if ((overall_mode == MODE_LOOK_TOWN) || (overall_mode == MODE_LOOK_COMBAT))
						if (adjacent(c_town.p_loc,destination) == TRUE)
							if (adj_town_look(destination) == TRUE)
								need_redraw = TRUE;
					if (is_sign(ter_looked_at)) {
						print_buf();
						need_reprint = FALSE;
						k = 0;
						if (overall_mode == MODE_LOOK_TOWN) {
							while (k < 15) {
								if (destination == c_town.town.sign_locs[k]) {
									need_reprint = TRUE;
									if (adjacent(c_town.town.sign_locs[k],c_town.p_loc)==TRUE)
										do_sign(c_town.town_num,k,(short) ter_looked_at,destination);
										else add_string_to_buf("  Too far away to read sign.      ");
									}
								k++;
								}
							}
						if (overall_mode == MODE_LOOK_OUTDOORS) {
							loc_in_sec = party.loc_in_sec;
							loc_in_sec.x += i - 4;
							loc_in_sec.y += j - 4;
							while (k < 8) {
								if (loc_in_sec == outdoors[party.i_w_c.x][party.i_w_c.y].sign_locs[k]) {
										need_reprint = TRUE;
										if (adjacent(outdoors[party.i_w_c.x][party.i_w_c.y].sign_locs[k],party.loc_in_sec)==TRUE)
											do_sign((short) (200 + get_outdoor_num()),k,(short) ter_looked_at,destination);
											else add_string_to_buf("  Too far away to read sign.      ");	
									}
								k++;
								}
							}
						}

				}
//			sprintf(store_str,"  Mod: %d",event.modifiers);
//			add_string_to_buf(store_str);

			// If option not pressed, looking done, so restore center
			if ((event.modifiers & 2048) == 0) {
				if (overall_mode == MODE_LOOK_COMBAT) {
					overall_mode = MODE_COMBAT;
					center = pc_pos[current_pc];
					pause(5);
					need_redraw = TRUE;
					}
				else if (overall_mode == MODE_LOOK_TOWN) {
					overall_mode = MODE_TOWN;
					center = c_town.p_loc;
					need_redraw = TRUE;
					}
				else if (overall_mode == MODE_LOOK_OUTDOORS)
					overall_mode = MODE_OUTDOORS;

				}
			}
				
// End: looking at something

// Begin : talking to someone			
			if (overall_mode == MODE_TALK_TOWN) { 
				destination.x = destination.x + i - 4;
				destination.y = destination.y + j - 4;

				if ((can_see(cur_loc,destination,0) >= 4) || (loc_off_world(destination) == TRUE)) {
					add_string_to_buf("  Can't see space              ");
					need_reprint = TRUE;
					}
				else {

					for (i = 0; i < T_M; i++) {
						if (monst_on_space(destination,i) == TRUE) {
							did_something = TRUE;
							need_redraw = TRUE;
							if (c_town.monst.dudes[i].attitude % 2 == 1) {
									add_string_to_buf("  Creature is hostile.        ");
									}
								else if ((c_town.monst.dudes[i].summoned > 0)////
									|| (c_town.monst.dudes[i].monst_start.personality < 0))
									add_string_to_buf("Talk: No response.            ");
								else switch (c_town.monst.dudes[i].monst_start.personality) {
									default: 
										start_talk_mode(i,c_town.monst.dudes[i].monst_start.personality,c_town.monst.dudes[i].number,
											c_town.monst.dudes[i].monst_start.facial_pic);
										did_something = FALSE;
										need_redraw = FALSE;				
										break;
									}
							}
						}
					if ((did_something == FALSE) && (overall_mode != MODE_TALKING)){
						add_string_to_buf("  Nobody there");
						need_reprint = TRUE;
					}
				if (overall_mode != MODE_TALKING)
					overall_mode = MODE_TOWN;
				}
				if (overall_mode != MODE_TALKING)
					need_redraw = TRUE;
			}			
// End : talking to someone

// Begin : Targeting a space
			if ((overall_mode == MODE_SPELL_TARGET) || (overall_mode == MODE_FIRING) || (overall_mode == MODE_THROWING) || 
			 (overall_mode == MODE_FANCY_TARGET) || (overall_mode == MODE_DROPPING)) {
				destination.x = destination.x + i - 4;
				destination.y = destination.y + j - 4;
				if (overall_mode == MODE_SPELL_TARGET)
					do_combat_cast(destination);
				if ((overall_mode == MODE_THROWING) || (overall_mode == MODE_FIRING))
					fire_missile(destination);
				if (overall_mode == MODE_FANCY_TARGET) {
					place_target(destination);
					need_reprint = TRUE;
					}
				if (overall_mode != MODE_FANCY_TARGET) {
					did_something = TRUE;
					center = pc_pos[current_pc];
					}
				if (overall_mode == MODE_DROPPING) { // dropping
					if (adjacent(pc_pos[current_pc],destination) == FALSE)
						add_string_to_buf("Drop: must be adjacent.");
						else {
							drop_item(current_pc,store_drop_item,destination);
							take_ap(1);
							}
					}
				pause(6);
				need_redraw = TRUE;
				if ((overall_mode >= MODE_COMBAT) && (overall_mode < MODE_TALKING) && (overall_mode != MODE_FANCY_TARGET))
					overall_mode = MODE_COMBAT;
				put_pc_screen();
				put_item_screen(stat_window,0);
				}
			if ((overall_mode > MODE_TALK_TOWN) && (overall_mode < MODE_COMBAT/*6*/)) {
				destination.x = destination.x + i - 4;
				destination.y = destination.y + j - 4;
				switch (overall_mode) {
					case 3: 
						cast_town_spell(destination);
						did_something = TRUE;
					break;
					case 4:
						if (adjacent(destination,c_town.p_loc) == FALSE)
							add_string_to_buf("  Must be adjacent.              ");
							else {
								did_something = use_space(destination);
								}
					break;
					case 5:
						if (adjacent(c_town.p_loc,destination) == FALSE)
							add_string_to_buf("Drop: must be adjacent.");
							else if (get_obscurity(destination.x,destination.y) == 5)
							ASB("Drop: Space is blocked.");
							else drop_item(current_pc,store_drop_item,destination);
						break;
					}
				overall_mode = MODE_TOWN;
				need_redraw = TRUE;
				put_pc_screen();
				put_item_screen(stat_window,0);
				}
// End : Targeting a space

		}
//End: click in terrain		

// Begin: Screen shift
	if ((overall_mode == MODE_SPELL_TARGET) ||  (overall_mode == MODE_FIRING) || (overall_mode == MODE_THROWING) || (overall_mode == MODE_FANCY_TARGET)
		|| (overall_mode == MODE_LOOK_COMBAT) || (overall_mode == MODE_LOOK_TOWN)) {
		if ((PtInRect (the_point, &border_rect[0])) && (center.y > c_town.town.in_town_rect.top)
		&& (center.y > 4)) {
			center.y--;
			need_redraw = TRUE;
			}
		if ((PtInRect (the_point, &border_rect[1])) && (center.x > c_town.town.in_town_rect.left)
		&& (center.x > 4)) {
			center.x--;
			need_redraw = TRUE;
			}
		if ((PtInRect (the_point, &border_rect[2])) && (center.y < c_town.town.in_town_rect.bottom)
		&& (center.y < town_size[town_type] - 5)) {
			center.y++;
			need_redraw = TRUE;
			}
		if ((PtInRect (the_point, &border_rect[3])) && (center.x < c_town.town.in_town_rect.right)
		&& (center.x < town_size[town_type] - 5)) {
			center.x++;
			need_redraw = TRUE;
			}
		} 
// End: Screen shift 


	// Process clicks in PC stats area
	if (PtInRect (the_point,&pc_area_rect)) { 
		point_in_area = the_point;
		point_in_area.h -= PC_WIN_UL_X;
		point_in_area.v -= PC_WIN_UL_Y;
		if (PtInRect (point_in_area,&pc_help_button)) {
			OffsetRect(&pc_help_button,PC_WIN_UL_X,PC_WIN_UL_Y);
			arrow_button_click(pc_help_button);
			OffsetRect(&pc_help_button,-1 * PC_WIN_UL_X,-1 * PC_WIN_UL_Y);
			FCD(1082,0);
			}
		for (i = 0; i < 6; i++)
			for (j = 0; j < 5; j++) 
				if ((pc_area_button_active[i][j] > 0) && (PtInRect (point_in_area,&pc_buttons[i][j]))) {
					OffsetRect(&pc_buttons[i][j],PC_WIN_UL_X,PC_WIN_UL_Y);
					arrow_button_click(pc_buttons[i][j]);
					OffsetRect(&pc_buttons[i][j],-1 * PC_WIN_UL_X,-1 * PC_WIN_UL_Y);
					switch (j) {
						case 0:
							if ((prime_time() == FALSE) && (overall_mode != MODE_SHOPPING)&& (overall_mode != MODE_TALKING))
								add_string_to_buf("Set active: Finish what you're doing first.");
								else if (is_combat())
								add_string_to_buf("Set active: Can't set this in combat.");
								else if ((adven[i].main_status != 1) &&
									((overall_mode != MODE_SHOPPING) || (store_shop_type != 3)))
								add_string_to_buf("Set active: PC must be here & active.");
							else {
									current_pc = i;
									set_stat_window (i);
									if (overall_mode == MODE_SHOPPING)
										sprintf((char *) str,"Now shopping: %s",adven[i].name);
										else sprintf((char *) str,"Now active: %s",adven[i].name);
									add_string_to_buf((char *)str);
									adjust_spell_menus();
									}
							break;
						case 1:
							sprintf((char *) str,"%s has %d health out of %d.",adven[i].name,
								adven[i].cur_health,adven[i].max_health);
							add_string_to_buf((char *)str);
							break;
						case 2:
							sprintf((char *) str,"%s has %d spell pts. out of %d.",adven[i].name,
								adven[i].cur_sp,adven[i].max_sp);
							add_string_to_buf((char *)str);
							break;
						case 3: // pc info
							give_pc_info(i);
							break;
						case 4: // trade places
							if (prime_time() == FALSE)
								add_string_to_buf("Trade places: Finish what you're doing first.");
								else if (is_combat())
								add_string_to_buf("Trade places: Can't do this in combat.");
								else {
									switch_pc(i);
									}						
							break;
						}
					}
		need_reprint = TRUE;
		put_pc_screen();
		put_item_screen(stat_window,0);
		if (overall_mode == MODE_SHOPPING) {
			set_up_shop_array();
			draw_shop_graphics(0,pc_buttons[0][0]);
			}
		}


	// Process clicks in item stats area
	if (PtInRect (the_point,&item_area_rect)) { 
		point_in_area = the_point;
		point_in_area.h -= ITEM_WIN_UL_X;
		point_in_area.v -= ITEM_WIN_UL_Y;

		for (i = 0; i < 9; i++)
			if ((item_bottom_button_active[i] > 0) && (PtInRect (point_in_area,&item_screen_button_rects[i]))) {
				OffsetRect(&item_screen_button_rects[i],ITEM_WIN_UL_X,ITEM_WIN_UL_Y);
				arrow_button_click(item_screen_button_rects[i]);
				OffsetRect(&item_screen_button_rects[i],-1 * ITEM_WIN_UL_X,-1 * ITEM_WIN_UL_Y);
				switch (i) {
					case 0: case 1: case 2: case 3: case 4: case 5:
						if ((prime_time() == FALSE) && (overall_mode != MODE_TALKING) && (overall_mode != MODE_SHOPPING))
							add_string_to_buf("Set active: Finish what you're doing first.");
							else {
								if (!(is_combat())) {
									if ((adven[i].main_status != 1) &&
									((overall_mode != MODE_SHOPPING) || (store_shop_type != 12)))
										add_string_to_buf("Set active: PC must be here & active.");
										else {
											current_pc = i;
											sprintf((char *) str,"Now active: %s",adven[i].name);
											add_string_to_buf((char *)str);
											adjust_spell_menus();
											}
								}
								set_stat_window(i);
								if (overall_mode == MODE_SHOPPING) {
									set_up_shop_array();
									draw_shop_graphics(0,item_screen_button_rects[i]); // rect is dummy
									}
								}
						break;
					case 6: // special screen
						give_help(50,0,0);
						set_stat_window(6);
						break;
					case 8: // help
						FCD(1090,0);
						break;
					}
				}
	if (stat_window < 7) {
		for (i = 0; i < 8; i++)
			for (j = 0; j < 6; j++) 
				if ((item_area_button_active[i][j] > 0) && (PtInRect (point_in_area,&item_buttons[i][j]))) {
					OffsetRect(&item_buttons[i][j],ITEM_WIN_UL_X,ITEM_WIN_UL_Y);
					arrow_button_click(item_buttons[i][j]);
					OffsetRect(&item_buttons[i][j],-1 * ITEM_WIN_UL_X,-1 * ITEM_WIN_UL_Y);

					item_hit = GetControlValue(item_sbar) + i;
					if ((prime_time() == FALSE) && (j < 4)
						&& ((j > 0) || (stat_screen_mode < 2)))
						add_string_to_buf("Item action: Finish what you're doing first.");
						else switch (j) {
							case 0: // equip
								if (overall_mode == MODE_USE) {
									add_string_to_buf("Note: Clicking 'U' button by item");
									add_string_to_buf("  uses the item.");
									use_item(stat_window, item_hit);
									overall_mode = MODE_TOWN;
									take_ap(3);
									}
									else if (prime_time() == TRUE) {
										equip_item(stat_window, item_hit);
										take_ap(1);
										}
								break;
							case 1: // use
								use_item(stat_window, item_hit);
								if ((overall_mode != MODE_TOWN_TARGET) && (overall_mode != MODE_SPELL_TARGET))
									did_something = TRUE;
								take_ap(3);
								break;
							case 2: // give
								give_thing(stat_window, item_hit);
								did_something = TRUE;
								take_ap(1);
								break;
							case 3: // drop
								if (stat_window == 6){
								use_spec_item(spec_item_array[item_hit]);
								need_redraw = TRUE;////
								}
								else if (is_out()) 
									drop_item(stat_window,item_hit,party.p_loc);
									else {
										add_string_to_buf("Drop item: Click where to drop item.");
										store_drop_item = item_hit;
										overall_mode = (is_town()) ? 5 : MODE_DROPPING;
										}
								break;
							case 4: // info
								if (stat_window == 6)
									put_spec_item_info(spec_item_array[item_hit]);
									else display_pc_item(stat_window, item_hit,adven[stat_window].items[item_hit],0);
								break;
							case 5: // sell? That this codes was reached indicates that the item was sellable
								switch (stat_screen_mode) {
									case 2: // identify item
										if (take_gold(shop_identify_cost,FALSE) == FALSE)
											ASB("Identify: You don't have the gold.");
											else {
												play_sound(68);
												ASB("Your item is identified.");
												adven[stat_window].items[item_hit].item_properties = 
													adven[stat_window].items[item_hit].item_properties | 1;
												}
										break;
									case 3: case 4: case 5: // various selling
										play_sound(-39);
										party.gold += store_selling_values[i];
										ASB("You sell your item.");
										take_item(stat_window,item_hit);
										break;
									case 6: // enchant item
										if (take_gold(store_selling_values[i],FALSE) == FALSE)
											ASB("Enchant: You don't have the gold.");
											else {
												play_sound(51);
												ASB("Your item is now enchanted.");
												enchant_weapon(stat_window,item_hit,shop_identify_cost,store_selling_values[i]);
												}
										break;
									}

								break;
							}
					}
		}
	put_pc_screen();
	put_item_screen(stat_window,0);
	need_reprint = TRUE;
	}


	// Alchemy
	if (the_point.h == 1000) {
		need_reprint = TRUE;
		need_redraw = TRUE;
		if (overall_mode == MODE_TOWN) {
			do_alchemy();
			}
			else add_string_to_buf("Alchemy: Only in town.");
		}


		
	// Wait 40 spaces (town only)
	if (the_point.h == 1001) {
		need_reprint = TRUE;
		need_redraw = TRUE;

		if (party_sees_a_monst() == TRUE)
			add_string_to_buf("Long wait: Monster in sight.");
			else {
				add_string_to_buf("Long wait...                    ");
				print_buf();
				play_sound(-20);
				draw_rest_screen();
				pause(10);
				for (i = 0; i < 6; i++) {
					store_sp[i] = adven[i].cur_health;
					adven[i].status[6] = 0;
					}
				}
		i = 0;			

		while ((i < 80) && (party_sees_a_monst() == FALSE)){
			increase_age();
			do_monsters();
			do_monster_turn();
			j = get_ran(1,1,160 - c_town.difficulty);
			if (j == 10)
				create_wand_monst();
			for (j = 0; j < 6; j++)
				if (adven[j].cur_health < store_sp[j]) {
					i = 200;
					j = 6;
					add_string_to_buf("  Waiting interrupted.");
					}
			if (party_sees_a_monst() == TRUE) {
				i = 200;
				add_string_to_buf("  Monster sighted!");
				}
				else i++;
				}
		put_pc_screen();		
		}

 	// If in combat and pc delayed, jump forward a step
 	if (pc_delayed == TRUE) {
 		initiate_redraw();
		//pause(2);
		current_pc++;
		combat_next_step();	
		set_stat_window(current_pc);		
		put_pc_screen();
 		}
 	
 	// At this point, see if any specials have been queued up, and deal with them
 	for (i = 0; i < 20; i++)
		if (special_queue[i] >= 0) {
			s3 = 0;
			switch (i) {
				case 0:
					run_special(5,2,special_queue[0],c_town.p_loc,&s1,&s2,&s3);
					break;
				case 1:
					run_special(6,2,special_queue[1],party.p_loc,&s1,&s2,&s3);
					break;
				}
			if (s3 > 0)
				initiate_redraw();
			}
				
	// Handle non-PC stuff (like monsters) if the party actually did something	
	if (did_something == TRUE) {
		draw_map(modeless_dialogs[5],5);

		if ((overall_mode >= MODE_COMBAT) && (overall_mode < MODE_TALKING)) {
			if (no_pcs_left() == TRUE) {
					end_combat();
					if (which_combat_type == 0) {
						end_town_mode(0,party.p_loc);
						add_string_to_buf("Fled the combat.        ");
						handle_wandering_specials(0,2);
						}
					}
				else {

					if (need_redraw == TRUE) {
						initiate_redraw();
						if ((combat_active_pc == 6) || (pc_moves[combat_active_pc] > 0))
							need_redraw = FALSE;
						}
					//pause(2);
					store_cur_pc = current_pc;
					if (combat_next_step() == TRUE)
						need_redraw = TRUE;
/*					if ((store_cur_pc != current_pc) && (combat_active_pc == 6)) {
			//			need_reprint = TRUE;
						need_redraw = TRUE;
						} */
						
					}
			}
		else {
			increase_age();
			if (!(is_out()) || ((is_out()) && (party.age % 10 == 0))) // no monst move is party outdoors and on horse
				do_monsters();
			if (overall_mode != MODE_OUTDOORS)
				do_monster_turn();
			// Wand monsts				
			if ((overall_mode == MODE_OUTDOORS) && (party_toast() == FALSE) && (party.age % 10 == 0)) {

				i = get_ran(1,1,70 + PSD[SDF_LESS_WANDER_ENC] * 200);
				if (i == 10)
					create_wand_monst();			
				for (i = 0; i < 10; i++)
					if (party.out_c[i].exists == TRUE)
						if (((adjacent(party.p_loc,party.out_c[i].m_loc) == TRUE) || 
							(party.out_c[i].what_monst.cant_flee >= 10))
							&& (party.in_boat < 0) && (flying() == FALSE)) {						
							store_wandering_special = party.out_c[i].what_monst;
							if (handle_wandering_specials(0,0) == TRUE)
								initiate_outdoor_combat(i);
							party.out_c[i].exists = FALSE;
							
							// Get rid of excess keyclicks
							FlushEvents(mDownMask,0);
							FlushEvents(keyDownMask,0);				

							need_reprint = FALSE; 
							i = 20;
							} 
				} 
			if (overall_mode == MODE_TOWN) {
				i = get_ran(1,1,160 - c_town.difficulty + PSD[SDF_LESS_WANDER_ENC] * 200);
				if (i == 2)
					create_wand_monst();
				}
				
			}


		}
		
	if (need_redraw == TRUE) {
		initiate_redraw();
		}


	if ((need_reprint == TRUE) || (need_redraw == TRUE)) {
		print_buf();
		}
		
	if (end_scenario == TRUE) {
		reload_startup();
		in_startup_mode = TRUE;
		draw_startup(0);
		menu_activate(1);		
		if (FCD(901,0) == 2)
			save_file(1);
		}
	else if (party_toast() == TRUE) {
		for (i = 0; i < 6; i++)
			if (adven[i].main_status == 5) {
				adven[i].main_status = 1;
				if (is_combat()) {
					end_town_mode(0,c_town.p_loc);
					add_string_to_buf("End combat.               ");
					handle_wandering_specials(0,2);

					}	
				}
		if (party.stuff_done[SDF_IS_PARTY_SPLIT] > 0) {
			end_split(0);
			if (is_combat()) {
				overall_mode = MODE_TOWN;
				}
				else if (is_town()) {
				
					}
			center = c_town.p_loc;
			}
		menu_activate(1);
		initiate_redraw();
		put_pc_screen();
		put_item_screen(stat_window,0);
		if (party_toast() == TRUE) {
				play_sound(13);
			handle_death();
			if (All_Done == TRUE)
				return TRUE;
			}
		}


	are_done = All_Done;
	return are_done;
}

Boolean someone_awake()
{
	short i;
	
	for (i = 0; i < 6; i++)
		if ((adven[i].main_status == 1) && (adven[i].status[11] <= 0) && (adven[i].status[12] <= 0))
			return TRUE;
	return FALSE;
}

void flash_rect(Rect to_flash)
{
	
	InvertRect (&to_flash);
	
	FlushAndPause(5);
	InvertRect (&to_flash);
}

void flash_round_rect(Rect to_flash,short radius)
{
	
	InvertRoundRect (&to_flash,radius,radius);
	if (play_sounds == TRUE)
		play_sound(37);
		else FlushAndPause(5);
	InvertRoundRect (&to_flash,radius,radius);

}

void button_flash_rect(Rect to_flash)
{
	InvertRect (&to_flash);
	if (play_sounds == TRUE)
		play_sound(34);
		else FlushAndPause(5);
	InvertRect (&to_flash);

}


void handle_menu_spell(short spell_picked,short spell_type) 
{
	Point pass_point;
	EventRecord event;
	
	spell_forced = TRUE;
	pc_casting = current_pc;
	pc_last_cast[spell_type][current_pc] = spell_picked;
	if (spell_type == 0)
		store_mage = spell_picked;
		else store_priest = spell_picked;
	if ((spell_type == 0) && (mage_need_select[spell_picked] > 0)) {
		if ((store_spell_target = char_select_pc(2 - mage_need_select[spell_picked],0,"Cast spell on who?")) == 6)
			return;
		}
		else {
			if ((spell_type == 1) && (priest_need_select[spell_picked] > 0))
				if ((store_spell_target = char_select_pc(2 - priest_need_select[spell_picked],0,"Cast spell on who?")) == 6)
					return;
			}
/*	if ((is_combat()) && (((spell_type == 0) && (refer_mage[spell_picked] > 0)) || 
	((spell_type == 1) && (refer_priest[spell_picked] > 0)))){
		if ((spell_type == 0) && (mage_need_select[spell_picked] > 0))
			store_spell_target = char_select_pc(2 - mage_need_select[spell_picked],0,"Cast spell on who?");
			else if ((spell_type == 1) && (priest_need_select[spell_picked] > 0))
				store_spell_target = char_select_pc(2 - priest_need_select[spell_picked],0,"Cast spell on who?");
		}
		else {
			} */
	pass_point.h = bottom_buttons[spell_type].left + 5;
	pass_point.v = bottom_buttons[spell_type].top + 5;
	AddPt(ul,&pass_point);
	event.where = pass_point;
	handle_action(event);
}

void initiate_outdoor_combat(short i)
{
	short m,n;
	location to_place;

	initiate_redraw();

	// Is combat too easy?
	if ((party_total_level() > ((out_enc_lev_tot(i) * 5) / 3) ) && (out_enc_lev_tot(i) < 200)
		&& (party.out_c[i].what_monst.cant_flee % 10 != 1)) {
		add_string_to_buf("Combat: Monsters fled!           ");
		party.out_c[i].exists = FALSE;
		return;
		}

//	Delay((long) 100,&dummy);
							
	start_outdoor_combat(party.out_c[i], out[party.p_loc.x][party.p_loc.y],count_walls(party.p_loc));

	party.out_c[i].exists = FALSE;

	for (m = 0; m < 6; m++)
		if (adven[m].main_status == 1)
			to_place = pc_pos[m];
	for (m = 0; m < 6; m++)
		for (n = 0; n < 24; n++)
			if ((adven[m].main_status != 1) && (adven[m].items[n].variety != 0)) {
				place_item(adven[m].items[n],to_place,TRUE);
				adven[m].items[n].variety = 0;
				}
							
	overall_mode = MODE_COMBAT;
	center = pc_pos[current_pc];
	initiate_redraw();
}

void initiate_redraw()
// Draw changes in terrain, but only if terrain is onscreen
{
	if ((overall_mode < MODE_TALKING) || (overall_mode >= MODE_LOOK_OUTDOORS/*30*/))
		draw_terrain(0);
}


Boolean handle_keystroke(char chr,char chr2,EventRecord event)
{
	Boolean are_done = FALSE;
	Point pass_point;
	short i,j;

	char keypad[10] = {82,83,84,85,86,87,88,89,91,92};
	Point terrain_click[10] = {{150,185},{120,215},{150,215},{180,215},
							{120,185},{150,185},{180,185},
								{120,155},{150,155},{180,135}};
	char talk_chars[9] = {'l','n','j','b','s','r','d','g','a'};
	Boolean dialog_grabbed_key = FALSE;
	
	if (FrontWindow() != mainPtr) {
		for (i = 0; i < 12; i++)
			if (modeless_exists[i] == TRUE)
				if ((FrontWindow() == GetDialogWindow(modeless_dialogs[i])) && ((chr == 13) || (chr2 == 76)|| (chr2 == 53))
					&& (overall_mode != MODE_TALKING) && (overall_mode != MODE_SHOPPING)) {
						//GetDialogItem(modeless_dialogs[i], 1, &the_type, &the_handle, &the_rect);
						//HiliteControl((ControlHandle)the_handle,inButton);
						//Delay(8,&dummy);
						//HiliteControl((ControlHandle)the_handle,0);						
						HideWindow(GetDialogWindow(modeless_dialogs[i])); 
						modeless_exists[i] = FALSE;				
						SelectWindow(mainPtr);
						SetPort(GetWindowPort(mainPtr));
						dialog_grabbed_key = TRUE;
					}
		if (dialog_grabbed_key == TRUE)
			return FALSE;
		}
		
	if (in_startup_mode == TRUE)
		return FALSE;
		
	ObscureCursor();
	SetPort(GetWindowPort(mainPtr));
	
// DEBUG
//	sprintf((char *) debug, "%d    ",(short) chr2); 
//	add_string_to_buf((char *) debug);
//	print_buf();
	if (overall_mode == MODE_TALKING) {
		if (chr2 == 53)
			chr = 'd';
		if (chr == ' ')
			chr = 'g';
		for (i = 0; i < 9; i++)
			if (chr == talk_chars[i]) {
				pass_point.h = preset_words[i].word_rect.left + 9;
				pass_point.v = preset_words[i].word_rect.top + 9;
				AddPt(ul,&pass_point);
				event.where = pass_point;
				are_done = handle_action(event);			
				}
		}
	else if (overall_mode == MODE_SHOPPING) { // shopping keystrokes
		if (chr2 == 53) {
				pass_point.h = 222;
				pass_point.v = 398;
				AddPt(ul,&pass_point);
				event.where = pass_point;
				are_done = handle_action(event);			
				}
		for (i = 0; i < 8; i++)
			if (chr == 97 + i) {
				pass_point.h = shopping_rects[i][1].left + 9;
				pass_point.v = shopping_rects[i][1].top + 9;
				AddPt(ul,&pass_point);
				event.where = pass_point;
				are_done = handle_action(event);			
				}
		}
		
	if ((overall_mode != MODE_TALKING) && (overall_mode != MODE_SHOPPING)) {
	for (i = 0; i < 10; i++)
		if (chr2 == keypad[i]) {
				if (i == 0) {
					chr = 'z';
					}
					else {
					pass_point.h = terrain_click[i].v;
					pass_point.v = terrain_click[i].h;
					AddPt(ul,&pass_point);
					event.where = pass_point;
					are_done = handle_action(event);
					return are_done;
					}
				}
		}
		
	switch(chr)
		{
	
		case '&':
			add_string_to_buf("If Valorim ...");
			print_buf();
			break;
		case '*':
			add_string_to_buf("You want to save ...");
			print_buf();
			break;
		case '(':
			add_string_to_buf("Back up your save files ...");
			print_buf();
			break;
		case ')':
			add_string_to_buf("Burma Shave.");
			print_buf();
			break;

		case '?':
			if (is_out()) FCD(1079,0);
			if (is_town()) FCD(1080,0); 
			if (is_combat()) FCD(1081,0);
			if (overall_mode == MODE_SHOPPING) {
				party.help_received[26] = 0;
				give_help(226,27,0);
				}
			if (overall_mode == MODE_SHOPPING) {
				party.help_received[26] = 0;
				give_help(226,27,0);
				}
			if (overall_mode == MODE_TALKING) {
				party.help_received[5] = 0;
				give_help(205,6,0);
				}
			break;

		case '1': case '2': case '3': case '4': case '5': case '6':
			pass_point.h = pc_buttons[((short) chr) - 49][0].left + 1 + PC_WIN_UL_X;
			pass_point.v = pc_buttons[((short) chr) - 49][0].top + PC_WIN_UL_Y;
			AddPt(ul,&pass_point);
			event.where = pass_point;
			are_done = handle_action(event);
		break;
		
		case '9':
			pass_point.h = item_screen_button_rects[6].left + ITEM_WIN_UL_X;
			pass_point.v = item_screen_button_rects[6].top + ITEM_WIN_UL_Y;
			AddPt(ul,&pass_point);
			event.where = pass_point;
			are_done = handle_action(event);
		break;

		case ' ':
			if (overall_mode == MODE_FANCY_TARGET) { // cast multi-target spell, set # targets to 0 so that
									// space clicked doesn't matter
				num_targets_left = 0;
				pass_point = terrain_click[5];
				AddPt(ul,&pass_point);
				event.where = pass_point;
				are_done = handle_action(event);			
				}
			if (overall_mode == MODE_SPELL_TARGET)
				spell_cast_hit_return();
		break;
		

		case 'D':
			if (in_scen_debug == TRUE)  {
				if (registered == FALSE) {
					ASB("Until your copy is registered,");
					ASB("you can't turn debug mode off.");
					break;
					}
				in_scen_debug = FALSE;
				ASB("Debug mode OFF.");
				}
				else if (enter_password() == FALSE) 
					ASB("Password incorrect.");
					else {
						in_scen_debug = TRUE;
						ASB("Debug mode ON.");
						}
			print_buf();
			break;
		case 'z': 
			if (((overall_mode >= MODE_COMBAT) && (overall_mode < MODE_TALKING)) || (overall_mode == MODE_LOOK_COMBAT)) {
				 set_stat_window(current_pc);
				put_item_screen(stat_window,0);
				}
				else {
					set_stat_window(0);
					put_item_screen(stat_window,0);								
					}
		break;

		case '=':

			for (i = 0; i < 6; i++) 
			for (j = 0; j < 30; j++) {
				adven[i].priest_spells[j] = 1;
				adven[i].mage_spells[j] = 1;
				}
			if (debug_on == FALSE) {
				break;
				}
			party.gold += 100;
			party.food += 100;
			for (i = 0; i < 6; i++) {
				adven[i].main_status = 1;
				adven[i].cur_health = adven[i].max_health;
				adven[i].cur_sp = 100;
				}
			award_party_xp(25);
			for (i = 0; i < 6; i++) 
			for (j = 0; j < 62; j++) {
				adven[i].priest_spells[j] = 1;
				adven[i].mage_spells[j] = 1;
				}
			refresh_store_items();
			add_string_to_buf("Debug: Add stuff and heal.            ");
			print_buf();
			put_pc_screen();
			break; 

		case 'K':
			if (debug_on) {
				for (i = 0; i < T_M; i++) {
				if ((is_combat()) && (c_town.monst.dudes[i].active > 0) && (c_town.monst.dudes[i].attitude % 2 == 1))
					c_town.monst.dudes[i].active = 0;
					
				if ((c_town.monst.dudes[i].active > 0) && (c_town.monst.dudes[i].attitude % 2 == 1)
				&& (dist(c_town.monst.dudes[i].m_loc,c_town.p_loc) <= 10) )
					damage_monst(i, 7,1000,0, 4);
					}
//				kill_monst(&c_town.monst.dudes[i],6);
				initiate_redraw();
				add_string_to_buf("Debug: Kill things.            ");
				print_buf();
				}
				
			break;   
			


		case 'W':
			refresh_store_items();
			add_string_to_buf("Debug: Refreshed jobs/shops.            ");
			print_buf();
			break; 

		case '`':
			//break;
			dump_gworld();
			break;
		case '[':
			//break;
			debug_ok = 1;
			return FALSE;
			break;
		case '+':
			//break;
			if (debug_on == TRUE) {
				print_nums(-999,-999,-999);
				debug_on = FALSE;
				}
			if (debug_ok == 1) {
				print_nums(999,999,999);
				debug_on = TRUE;
				}
			break;
		case '/':
			//break;
			if ((debug_on == TRUE) || (in_scen_debug == TRUE)) {
				ASB("Debug: Increase age.");
				ASB("  It is now 1 day later.");
				print_buf();
				party.age += 3700;
				put_pc_screen();
				}
			break;
		case '>':
			ASB("DEBUG: Towns have short memory.");
			ASB("Your deeds have been forgotten.");
			print_buf();
			for (i = 0; i < 4; i++)
				party.creature_save[i].which_town = 200;
			break;			
		case 'a':
			if (overall_mode < MODE_TALK_TOWN) {
				pass_point.h = (overall_mode == MODE_OUTDOORS) ? 170 : 221;
				pass_point.v = 405;
				AddPt(ul,&pass_point);
				event.where = pass_point;
				are_done = handle_action(event);
				}
				
			break;

		case 'b': case 'u': case 'L':
			if (overall_mode == MODE_TOWN) {
				pass_point.h = (chr == 'u') ? 220 : 205;
				pass_point.v = (chr == 'L') ? 405 : 388;
				AddPt(ul,&pass_point);
				event.where = pass_point;
				are_done = handle_action(event);
				}
			break;

		case 's': case 'x': case 'e':
			if ((overall_mode == MODE_COMBAT) ||
				((overall_mode == MODE_FIRING)  && (chr == 's')) ||
				((overall_mode == MODE_THROWING)  && (chr == 's')) ) {
				pass_point.h = (chr == 's') ? 205 : 240;
				pass_point.v = (chr == 'e') ? 390 : 406;
				AddPt(ul,&pass_point);
				event.where = pass_point;
				are_done = handle_action(event);
				}
			break;
		
		
		case 'm': case 'p': case 'l': case 'r': case 'w': case 't': case 'd': case 'g': case 'f':
		case 'M': case 'P': case 'A':
			j = 50;
			if ((chr == 'm') && ((overall_mode == MODE_SPELL_TARGET) || (overall_mode == MODE_FANCY_TARGET))) // cancel spell 
				j = 0;
			if ((chr == 'p') && ((overall_mode == MODE_SPELL_TARGET) || (overall_mode == MODE_FANCY_TARGET))) // cancel spell 
				j = 1;
			if ((overall_mode == MODE_OUTDOORS) || (overall_mode == MODE_TOWN) || (overall_mode == MODE_COMBAT)) {
			switch (chr) {
				case 'M': spell_forced = TRUE; j = 0; break;
				case 'm': j = 0; break;
				case 'P': spell_forced = TRUE; j = 1; break;
				case 'p': j = 1; break;
				case 'l': j = 2; break;
				case 'r': if (overall_mode != MODE_OUTDOORS) return FALSE;
					j = 3;
					break;
				case 't': if (overall_mode == MODE_TOWN)
							j = 3;
								else return FALSE;
					break;
				case 'A':if (overall_mode == MODE_TOWN) {
									pass_point.h = 1000 + ul.h;
									event.where = pass_point;
									are_done = handle_action(event);								
									}
								else {
									add_string_to_buf("Alchemy: In town only.");
									print_buf();
									return FALSE;
									}
					break;
				case 'w':if (overall_mode == MODE_COMBAT)
								j = 5;
								else if (overall_mode == MODE_TOWN) {
									pass_point.h = 1001 + ul.h;
									event.where = pass_point;
									are_done = handle_action(event);								
									}
								else {
									add_string_to_buf("Wait: In town only.");
									print_buf();
									return FALSE;
									}
					break;
				case 'd': if (overall_mode != MODE_COMBAT) return FALSE;
					j = 3;
					break;
				case 'g': if (overall_mode == MODE_OUTDOORS) return FALSE;
					j = 4;
					break;
				case 'f': if (overall_mode != MODE_TOWN) return FALSE;
					j = 6;
					break;
				}
				}
			if (j < 50) {
				pass_point.h = bottom_buttons[j].left + 5;
				pass_point.v = bottom_buttons[j].top + 5;
				AddPt(ul,&pass_point);
				event.where = pass_point;
				are_done = handle_action(event);
				}
			break;  
			}
			
		
	debug_ok = 0;
	
	spell_forced = FALSE;
	return are_done;
}


void menu_activate( short type)
// type;  // 0 - save off alter off 1 - not all off
{
	MenuHandle cur_menu;
	
	if (in_startup_mode == TRUE) {
		cur_menu = GetMenuHandle(600);
		DisableMenuItem(cur_menu,0);
		cur_menu = GetMenuHandle(700);
		DisableMenuItem(cur_menu,0);
		cur_menu = GetMenuHandle(800);
		DisableMenuItem(cur_menu,0);
		cur_menu = GetMenuHandle(900);
		DisableMenuItem(cur_menu,0);
		cur_menu = GetMenuHandle(950);
		DisableMenuItem(cur_menu,0);


		cur_menu = GetMenuHandle(550);
		if (party_in_memory == FALSE) {
			DisableMenuItem(cur_menu,2);
			DisableMenuItem(cur_menu,3);
			}
			else {
				DisableMenuItem(cur_menu,2);
				EnableMenuItem(cur_menu,3);			
				}
		DrawMenuBar();
		return;
		}

	cur_menu = GetMenuHandle(600);
	EnableMenuItem(cur_menu,0);
	cur_menu = GetMenuHandle(700);
	EnableMenuItem(cur_menu,0);
	cur_menu = GetMenuHandle(800);
	EnableMenuItem(cur_menu,0);
	cur_menu = GetMenuHandle(900);
	EnableMenuItem(cur_menu,0);
	cur_menu = GetMenuHandle(950);
	EnableMenuItem(cur_menu,0);

	cur_menu = GetMenuHandle(550);
	EnableMenuItem(cur_menu,2);
	EnableMenuItem(cur_menu,3);
	DrawMenuBar();
	
}

void do_load()
{
	load_file();
	if (in_startup_mode == FALSE)
		post_load();
	menu_activate(1);
}

void post_load()
{
	
	//SelectWindow(mainPtr);
	SetPort(GetWindowPort(mainPtr));
	current_switch = 6;

	reset_item_max();

	if (overall_mode == MODE_OUTDOORS)
		update_explored(party.p_loc);
	if (overall_mode == MODE_TOWN) {
		make_town_trim(0);
		}
	make_out_trim();
	
	ShowControl(text_sbar);
	Draw1Control(text_sbar);
	ShowControl(item_sbar);
	Draw1Control(item_sbar);
	HideControl(shop_sbar);
	set_stat_window(0);
	put_pc_screen();
	initiate_redraw();
	draw_buttons(0);
	draw_text_bar(1);

	print_buf();
	
	clear_map();
	adjust_spell_menus();
	adjust_monst_menu();
	
	cartoon_happening = FALSE;
}

void do_save(short mode)
//mode; // 0 - normal  1 - save as
{
//	SelectWindow(mainPtr);
	if (overall_mode > MODE_TOWN) {
		add_string_to_buf("Save: Only while outdoors, or in         ");
		add_string_to_buf("  town and not looking/casting.          ");
		print_buf();
		return;
		}
	save_file(mode);
	pause(6);
	SetPort(GetWindowPort(mainPtr));
	initiate_redraw();
	put_pc_screen();
	put_item_screen(stat_window,0);
	print_buf();
}
			
void increase_age()////
{
	short i,j,item,how_many_short = 0,r1,store_day;
	Boolean update_stat = FALSE;

			
	// Increase age, adjust light level & stealth
	store_day = calc_day();
	if (is_out()) {
		if (party.in_horse < 0) 
			party.age -= party.age % 10;
			else party.age -= party.age % 5;
		party.age += 5;
		if (party.in_horse < 0) 
			party.age += 5;
			
		}
		else party.age++;
	if (calc_day() != store_day) { // Day changed, so check for interesting stuff.
		update_stat = TRUE;
		}
		
	party.light_level = move_to_zero(party.light_level);
	
//	if (party.stuff_done[128][9] == 1)
//		clear_map();

	// decrease monster present counter
	party.stuff_done[SDF_HOSTILES_PRESENT] = move_to_zero(party.stuff_done[SDF_HOSTILES_PRESENT]);

	// Party spell effects
	if (party.stuff_done[SDF_PARTY_STEALTHY] == 1) {reset_text_bar();
		add_string_to_buf("Your footsteps grow louder.      "); }
	party.stuff_done[SDF_PARTY_STEALTHY] = move_to_zero(party.stuff_done[SDF_PARTY_STEALTHY]);
	if (party.stuff_done[SDF_PARTY_DETECT_LIFE] == 1) {reset_text_bar();
		add_string_to_buf("You stop detecting monsters.      ");}
	party.stuff_done[SDF_PARTY_DETECT_LIFE] = move_to_zero(party.stuff_done[SDF_PARTY_DETECT_LIFE]);
	if (party.stuff_done[SDF_PARTY_FIREWALK] == 1) {reset_text_bar();
		add_string_to_buf("Your feet stop glowing.      ");}
	party.stuff_done[SDF_PARTY_FIREWALK] = move_to_zero(party.stuff_done[SDF_PARTY_FIREWALK]);

	if (party.stuff_done[SDF_PARTY_FLIGHT] == 2) 
		add_string_to_buf("You are starting to descend.");
	if (party.stuff_done[SDF_PARTY_FLIGHT] == 1) {
		if (scenario.ter_types[out[party.p_loc.x][party.p_loc.y]].blockage > 2) { 
				add_string_to_buf("  You plummet to your deaths.                  ");
				slay_party(2);
				print_buf();
				pause(150);
			}
			else add_string_to_buf("  You land safely.                  ");
		reset_text_bar();
	}
				
	party.stuff_done[SDF_PARTY_FLIGHT] = move_to_zero(party.stuff_done[SDF_PARTY_FLIGHT]);

	if ((overall_mode > MODE_OUTDOORS) && (c_town.town.lighting == 2))
		party.light_level = max (0,party.light_level - 9);
	if (c_town.town.lighting == 3) {
		if (party.light_level > 0)
			ASB("Your light is drained.");
		party.light_level = 0;
		}


	// Specials countdowns
	if ((party.age % 500 == 0) && (get_ran(1,0,5) == 3) && (party_has_abil(52) == TRUE)) {
			update_stat = TRUE;
			display_enc_string(52,39,3);
			for (i = 0; i < 6; i++)
				disease_pc(i,2);	
		}
	

	// Plants and magic shops	
	if (party.age % 4000 == 0) {
		//SysBeep(2);
		//ASB("DEBUG: Stuff replaced.");
		refresh_store_items();
		//for (i = 0; i < 10; i++)
		//	party.stuff_done[302][i] = 0;
		//for (i = 0; i < 10; i++)
		//	party.stuff_done[301][i] = 0;
		//for (i = 0; i < 10; i++)
		////	party.stuff_done[300][i] = 0;
		}
		
	// Protection, etc.
	for (i = 0; i < 6; i++) { // Process some status things, and check if stats updated
	
			if ((adven[i].status[4] == 1) || (adven[i].status[5] == 1) || (adven[i].status[8] == 1)
			|| (adven[i].status[11] == 1)|| (adven[i].status[12] == 1))
				update_stat = TRUE;
 			adven[i].status[4] = move_to_zero(adven[i].status[4]);
			adven[i].status[5] = move_to_zero(adven[i].status[5]);	
			adven[i].status[8] = move_to_zero(adven[i].status[8]);	
			adven[i].status[10] = move_to_zero(adven[i].status[10]);	
			adven[i].status[11] = move_to_zero(adven[i].status[11]);	
			adven[i].status[12] = move_to_zero(adven[i].status[12]);	
			if ((party.age % 40 == 0) && (adven[i].status[0] > 0)) {
				update_stat = TRUE;
 				adven[i].status[0] = move_to_zero(adven[i].status[0]);
 				}

		}

	// Food
	if ((party.age % 1000 == 0) && (overall_mode < MODE_COMBAT)) {
		for (i = 0; i < 6; i++)
			if (adven[i].main_status == 1)
				how_many_short++;
			how_many_short = take_food (how_many_short,FALSE);
			if (how_many_short > 0) {
				add_string_to_buf("Starving! ");
				play_sound(66);
				r1 = get_ran(3,1,6);
				hit_party(r1,4);
				update_stat = TRUE;
				if (overall_mode < MODE_COMBAT)
					boom_space(party.p_loc,overall_mode,0,r1,0);
				}
				else {
					play_sound(6);
					add_string_to_buf("You eat.  ");
					}
			update_stat = TRUE;
		}

	// Poison, acid, disease damage
	for (i = 0; i < 6; i++) // Poison
		if (adven[i].status[2] > 0) {
			i = 6;
			if (((overall_mode == MODE_OUTDOORS) && (party.age % 50 == 0)) || ((overall_mode == MODE_TOWN) && (party.age % 20 == 0))) {
				update_stat = TRUE;
				do_poison();
				}
			}
	for (i = 0; i < 6; i++) // Disease
		if (adven[i].status[7] > 0) {
			i = 6;
			if (((overall_mode == MODE_OUTDOORS) && (party.age % 100 == 0)) || ((overall_mode == MODE_TOWN) && (party.age % 25 == 0))) {
				update_stat = TRUE;
				handle_disease();
				}
			}
	for (i = 0; i < 6; i++) // Acid
		if (adven[i].status[13] > 0) {
			i = 6;
			update_stat = TRUE;
			handle_acid();
			}
		
	// Healing and restoration of spell pts.
	if (is_out()) {
			if (party.age % 100 == 0) {
				for (i = 0; i < 6; i++)
					if ((adven[i].main_status == 1) && (adven[i].cur_health < adven[i].max_health))
						update_stat = TRUE;
				heal_party(2);
				}
			}
		else {
			if (party.age % 50 == 0) {
				for (i = 0; i < 6; i++)
					if ((adven[i].main_status == 1) && (adven[i].cur_health < adven[i].max_health))
						update_stat = TRUE;
				heal_party(1);
				}
			}
	if (is_out()) {
			if (party.age % 80 == 0) {
			for (i = 0; i < 6; i++)
				if ((adven[i].main_status == 1) && (adven[i].cur_sp < adven[i].max_sp))
					update_stat = TRUE;
				restore_sp_party(2);
				}
			}
		else {	
			if (party.age % 40 == 0) {
			for (i = 0; i < 6; i++)
				if ((adven[i].main_status == 1) && (adven[i].cur_sp < adven[i].max_sp))
					update_stat = TRUE;
				restore_sp_party(1);
				}
			}

	// Recuperation and chronic disease disads
	for (i = 0; i < 6; i++) 
		if (adven[i].main_status == 1) {
			if ((adven[i].traits[9] > 0) && (get_ran(1,0,10) == 1) && (adven[i].cur_health < adven[i].max_health)) {
				heal_pc(i,2);
				update_stat = TRUE;
				}
			if ((adven[i].traits[13] > 0) && (get_ran(1,0,110) == 1)) {
				disease_pc(i,4);
				update_stat = TRUE;
				}
			
			}
			
				
	// Blessing, slowed,etc.
	if (party.age % 4 == 0) 
		for (i = 0; i < 6; i++) {
			if ((adven[i].status[1] != 0) || (adven[i].status[3] != 0))
				update_stat = TRUE;
			adven[i].status[1] = move_to_zero(adven[i].status[1]);
			adven[i].status[3] = move_to_zero(adven[i].status[3]);	
			if (((item = pc_has_abil_equip(i,50)) < 24) 
				&& (adven[i].cur_health < adven[i].max_health)
				&& ((overall_mode > MODE_OUTDOORS) || (get_ran(1,0,10) == 5))){
					j = get_ran(1,0,adven[i].items[item].ability_strength / 3);
					if (adven[i].items[item].ability_strength / 3 == 0)
						j = get_ran(1,0,1);
					if (is_out()) j = j * 4;
					heal_pc(i,j);	
					update_stat = TRUE;
					}
			}

	dump_gold(1);

	special_increase_age();
	//if (debug_on == FALSE)
	push_things();
		
	if (is_town()) {
		process_fields();
		}


		
	// Cancel switching PC order
	current_switch = 6;

	// If a change, draw stat screen
	if (update_stat == TRUE) 
		put_pc_screen();
	adjust_spell_menus();
}

void handle_cave_lore()////
{
	char str[60];
	short i,pic;
	unsigned char ter;
	
	if (!is_out())
		return;
	
	ter = out[party.p_loc.x][party.p_loc.y];
	pic = scenario.ter_types[ter].picture;
	for (i = 0; i < 6; i++)
		if ((adven[i].main_status == 1) && (adven[i].traits[4] > 0) && (get_ran(1,0,12) == 5)
			&& (((pic >= 0) && (pic <= 1)) || ((pic >= 70) && (pic <= 76))) ) {
			sprintf((char *)str,"%s hunts.",adven[i].name);
			party.food += get_ran(2,1,6);
			add_string_to_buf((char *)str);
			put_pc_screen();
			}
	for (i = 0; i < 6; i++)
		if (
		(adven[i].main_status == 1) && (adven[i].traits[5] > 0) && (get_ran(1,0,12) == 5)
			&& (((pic >= 2) && (pic <= 4)) || ((pic >= 79) && (pic <= 84)))) {
			sprintf((char *)str,"%s hunts.",adven[i].name);
			party.food += get_ran(2,1,6);
			add_string_to_buf((char *)str);
			put_pc_screen();
			}
			

}

void switch_pc(short which)
{
	pc_record_type store_pc;

	if (current_switch < 6) {
		add_string_to_buf("Switch: OK.");
		if (current_switch != which) {
			store_pc = adven[which];
			adven[which] = adven[current_switch];
			adven[current_switch] = store_pc;
			if (current_pc == current_switch)
				current_pc = which;
				else if (current_pc == which)
				current_pc = current_switch;
			set_stat_window(current_pc);
			update_pc_graphics();
			current_switch = 6;
			}
		}
	else {
		add_string_to_buf("Switch: Switch with who?    ");
		current_switch = which;
		}
}

void drop_pc(short which)
{
	short choice,i;
	
	choice = fancy_choice_dialog(1053,0);
	if (choice == 1) {
		add_string_to_buf("Delete PC: Cancelled.           ");
		return;
		}
	add_string_to_buf("Delete PC: OK.                  ");
	kill_pc(which,0);
	for (i = which; i < 5; i++)
		adven[i] = adven[i + 1];
	adven[5].main_status = 0;
	set_stat_window(0);
	put_pc_screen();
}

void handle_death()
{
	short choice;

	in_startup_mode = TRUE;

	while (in_startup_mode == TRUE) {
		// Use death (or leave Exile) dialog
		in_startup_mode = FALSE;
		choice = FCD(1069,0); //// FCD hates it when it gets called in startup mode
					// and startup graphiocs aren't loaded.
		in_startup_mode = TRUE;

		if (choice == 3) {
			in_startup_mode = FALSE;
			All_Done = TRUE;
			}
		if (choice == 1) {
			in_startup_mode = FALSE;
			load_file();
			if (party_toast() == FALSE) {
				if (in_startup_mode == FALSE)
					post_load();
            	else return;
				}
				else in_startup_mode = TRUE;
			}
		if (choice == 2) {
			start_new_game();
			}
		}
	
}

void start_new_game()
{
	short i;
	short choice;

	choice = FCD(1065,0);
	if (choice == 2)
		return;

	//which = choice - 1;
	
//	display_intro();
	init_party(0);

	//while (creation_done == FALSE) {
		edit_party(1,0);
	/*	if ((i > 0) || (in_startup_mode == FALSE))
			creation_done = TRUE;
		if ((i == 0) && (in_startup_mode == FALSE))
			return;
		} */
	
	// if no PCs left, forget it
	for (i = 0 ; i < 6; i++)
		if (adven[i].main_status == 1)
			i = 100;
	if (i == 6)
		return;

	
	// everyone gets a weapon
	for (i = 0; i < 6; i++)
		if (adven[i].main_status == 1) {
			adven[i].items[0] = start_items[adven[i].race * 2];
			adven[i].equip[0] = TRUE;
			adven[i].items[1] = start_items[adven[i].race * 2 + 1];
			adven[i].equip[1] = TRUE;
			}
	// PCs get adjustments
	for (i = 0; i < 6; i++)
		if (adven[i].main_status == 1) {
			// Do stat adjs for selected race.
			if (adven[i].race == 1)
				adven[i].skills[1] += 2;
			if (adven[i].race == 2) {
				adven[i].skills[0] += 2;
				adven[i].skills[2] += 1;	
				}
			adven[i].max_sp += adven[i].skills[9] * 3 + adven[i].skills[10] * 3;
			adven[i].cur_sp = adven[i].max_sp;
			}
	save_file(1);
	party_in_memory = TRUE;

	}

location get_cur_direction(Point the_point)
{
	location store_dir = {0,0};

	// This is a kludgy adjustment to adjust for the screen shifting between Exile I & II
	the_point.v += 5;
	the_point.h += 5;

				if ((the_point.h < 135) & (the_point.v >= ((the_point.h * 34) / 10) - 293)
					& (the_point.v <= (-1 * ((the_point.h * 34) / 10) + 663)))
					store_dir.x--;
				if ((the_point.h > 163) & (the_point.v <= ((the_point.h * 34) / 10) - 350)
					& (the_point.v >= (-1 * ((the_point.h * 34) / 10) + 721)))
					store_dir.x++;
					
				if ((the_point.v < 167) & (the_point.v <= (the_point.h / 2) + 102)
					& (the_point.v <= (-1 * (the_point.h / 2) + 249)))
					store_dir.y--;
				if ((the_point.v > 203) & (the_point.v >= (the_point.h / 2) + 123)
					& (the_point.v >= (-1 * (the_point.h / 2) + 268)))
					store_dir.y++;
					
	return store_dir;
}

Boolean outd_move_party(location destination,Boolean forced)
{
	char create_line[60];
	short boat_num,horse_num,spec_num;
	location real_dest, sector_p_in;
	Boolean keep_going = TRUE,check_f;
	location store_corner,store_iwc;
	unsigned char ter;
	
	keep_going = check_special_terrain(destination,0,0,&spec_num,&check_f);
	if (check_f == TRUE)
		forced = TRUE;
	if (debug_on == TRUE)
		forced = TRUE;
	if (spec_num == 50)
		forced = TRUE;

	// If not blocked and not put in town by a special, process move
	if ((keep_going == TRUE) && (overall_mode == MODE_OUTDOORS)) {

	real_dest.x = destination.x - party.p_loc.x;
	real_dest.y = destination.y - party.p_loc.y;

	sector_p_in.x = party.outdoor_corner.x + party.i_w_c.x;
	sector_p_in.y = party.outdoor_corner.y + party.i_w_c.y;

	store_corner = party.outdoor_corner;
	store_iwc = party.i_w_c;
	
	// Check if party moves into new sector
	if ((destination.x < 6) && (party.outdoor_corner.x > 0)) 
			shift_universe_left();
	if ((destination.x > 90) && (party.outdoor_corner.x < scenario.out_width - 1))
			shift_universe_right();
	if ((destination.y < 6)  && (party.outdoor_corner.y > 0)) {
			shift_universe_up();
			}
	else if ((destination.y > 90)  && (party.outdoor_corner.y < scenario.out_height - 1))
			shift_universe_down();      
	// Now stop from going off the world's edge
	real_dest.x = party.p_loc.x + real_dest.x; 
	real_dest.y = party.p_loc.y + real_dest.y; 
	if ((real_dest.x < 1 /*4*/) && (party.outdoor_corner.x <= 0)) {
		ASB("You've reached the world's edge.");
		return FALSE;
	}
	if (((real_dest.x > 95 /*92*/) && (party.outdoor_corner.x >= scenario.out_width - 2)) ||
		((real_dest.x > 47 /*44*/) && (party.outdoor_corner.x >= scenario.out_width - 1))) {
		ASB("You've reached the world's edge.");
		return FALSE;
	}
	if ((real_dest.y < 1 /*4*/)  && (party.outdoor_corner.y <= 0)) {
		ASB("You've reached the world's edge.");
		return FALSE;
	}
	else if (((real_dest.y > 95 /*92*/)  && (party.outdoor_corner.y >= scenario.out_height - 2)) ||
			((real_dest.y > 47 /*44*/)  && (party.outdoor_corner.y >= scenario.out_height - 1))) {
		ASB("You've reached the world's edge.");
		return FALSE;
	}

//	if ((store_corner.x != party.outdoor_corner.x) || (store_corner.y != party.outdoor_corner.y) ||
//		(store_iwc.x != party.i_w_c.x) || (store_iwc.y != party.i_w_c.y))
//		clear_map();
				


	//if (forced == TRUE)
	//	for (i = 0; i < 10; i++)
	//		if (same_point(destination,party.out_c[i].m_loc) == TRUE)
	//				party.out_c[i].exists = FALSE;

		ter = out[real_dest.x][real_dest.y];
	if (party.in_boat >= 0) {
		if ((outd_is_blocked(real_dest) == FALSE) //&& (outd_is_special(real_dest) == FALSE)
		// not in towns
		&& ((scenario.ter_types[ter].boat_over == FALSE)
			|| ((real_dest.x != party.p_loc.x) && (real_dest.y != party.p_loc.y)))
			&& (scenario.ter_types[ter].special != 21)) {
					add_string_to_buf("You leave the boat.");
					party.in_boat = -1;
					}
			else if (((real_dest.x != party.p_loc.x) && (real_dest.y != party.p_loc.y))
				|| ((forced == FALSE) && (out_boat_there(destination) < 30)))
				return FALSE;
			else if ((outd_is_blocked(real_dest) == FALSE) 
				&& (scenario.ter_types[ter].boat_over == TRUE)
				&& (scenario.ter_types[ter].special != 21)) {
				if ((fancy_choice_dialog(1086,0)) == 1)
					forced = TRUE;
					else {
						add_string_to_buf("You leave the boat.             ");
						party.in_boat = -1;					
						}
				}
			else if (scenario.ter_types[ter].boat_over == TRUE)
				forced = TRUE;
		}

	if (((boat_num = out_boat_there(real_dest)) < 30) && (party.in_boat < 0) && (party.in_horse < 0)) {
		if (flying() == TRUE) {
			add_string_to_buf("You land first.                 ");
			party.stuff_done[SDF_PARTY_FLIGHT] = 0;
			}
			give_help(61,0,0);
			add_string_to_buf("Move: You board the boat.           ");
			party.in_boat = boat_num;
			party.direction = set_direction(party.p_loc, destination); 
		
			party.p_loc = real_dest;
			party.i_w_c.x = (party.p_loc.x > 48) ? 1 : 0;
			party.i_w_c.y = (party.p_loc.y > 48) ? 1 : 0;
			party.loc_in_sec = global_to_local(party.p_loc);

			if ((store_corner.x != party.outdoor_corner.x) || (store_corner.y != party.outdoor_corner.y) ||
				(store_iwc.x != party.i_w_c.x) || (store_iwc.y != party.i_w_c.y))
				clear_map();
			
			return TRUE;
		} 					
	else if (((horse_num = out_horse_there(real_dest)) < 30) && (party.in_boat < 0) && (party.in_horse < 0)) {
		if (flying() == TRUE) {
			add_string_to_buf("Land before mounting horses.");
			return FALSE;
			}
		if ((scenario.ter_types[ter].special >= 2) && (scenario.ter_types[ter].special <= 4)) {
			ASB("Your horses quite sensibly refuse.");
			return FALSE;
			}
			
			give_help(60,0,0);
			add_string_to_buf("Move: You mount the horses.           ");
			play_sound(84);
			party.in_horse = horse_num;
			party.direction = set_direction(party.p_loc, destination); 
		
			party.p_loc = real_dest;
			party.i_w_c.x = (party.p_loc.x > 48) ? 1 : 0;
			party.i_w_c.y = (party.p_loc.y > 48) ? 1 : 0;
			party.loc_in_sec = global_to_local(party.p_loc);

			if ((store_corner.x != party.outdoor_corner.x) || (store_corner.y != party.outdoor_corner.y) ||
				(store_iwc.x != party.i_w_c.x) || (store_iwc.y != party.i_w_c.y))
				clear_map();
			
			return TRUE;
		} 					
	else if ((outd_is_blocked(real_dest) == FALSE) || (forced == TRUE)
		// Check if can fly over
		|| ((flying() == TRUE) && 
			(scenario.ter_types[ter].fly_over == TRUE))   ) {
		party.direction = set_direction(party.p_loc, destination); 

		if ((flying() == TRUE) && (scenario.ter_types[ter].special == 21)) {
			add_string_to_buf("Moved: You have to land first.               ");
			return FALSE;
			}

		
		party.p_loc = real_dest;
		party.i_w_c.x = (party.p_loc.x > 47) ? 1 : 0;
		party.i_w_c.y = (party.p_loc.y > 47) ? 1 : 0;
		party.loc_in_sec = global_to_local(party.p_loc);
		sprintf ((char *) create_line, "Moved: %s",dir_string[party.direction]);//, party.p_loc.x, party.p_loc.y, party.loc_in_sec.x, party.loc_in_sec.y);
		add_string_to_buf((char *) create_line);
		move_sound(out[real_dest.x][real_dest.y],num_out_moves);
		num_out_moves++;
		
		if (party.in_boat >= 0) { // Waterfall!!!
			while (scenario.ter_types[out[party.p_loc.x][party.p_loc.y + 1]].special == 15) {
				add_string_to_buf("  Waterfall!                     ");
				party.p_loc.y += 2;
				party.loc_in_sec.y += 2;
				update_explored(party.p_loc);
				initiate_redraw();
				print_buf();
				if ((cave_lore_present() > 0) && (get_ran(1,0,1) == 0))
					add_string_to_buf("  (No supplies lost.)");
				else if (party.food > 1800){
					add_string_to_buf("  (Many supplies lost.)");
					party.food -= 50;
				}
				else {
					int n = party.food;
					char s[25];
					party.food = (party.food * 19) / 20;
					sprintf(s,"  (%d supplies lost.)",n - party.food);
					add_string_to_buf(s);
				}
				put_pc_screen();
				play_sound(28);
				pause(8);
			}			
			party.boats[party.in_boat].which_town = 200;
			party.boats[party.in_boat].loc_in_sec = party.loc_in_sec;
			party.boats[party.in_boat].loc = party.p_loc;
			party.boats[party.in_boat].sector.x = party.outdoor_corner.x + party.i_w_c.x;
			party.boats[party.in_boat].sector.y = party.outdoor_corner.y + party.i_w_c.y;
		}
		if (party.in_horse >= 0) {
				party.horses[party.in_horse].which_town = 200;
				party.horses[party.in_horse].loc_in_sec = party.loc_in_sec;
				party.horses[party.in_horse].loc = party.p_loc;
				party.horses[party.in_horse].sector.x = party.outdoor_corner.x + party.i_w_c.x;
				party.horses[party.in_horse].sector.y = party.outdoor_corner.y + party.i_w_c.y;

			}

		if ((store_corner.x != party.outdoor_corner.x) || (store_corner.y != party.outdoor_corner.y) ||
			(store_iwc.x != party.i_w_c.x) || (store_iwc.y != party.i_w_c.y))
			clear_map();
			
		return TRUE;
		}
		else {
			sprintf ((char *) create_line, "Blocked: %s",dir_string[set_direction(party.p_loc, destination)]);		
			add_string_to_buf((char *) create_line);	
			return FALSE;
			}
	}
	return FALSE;
}

Boolean town_move_party(location destination,short forced)////
{
	char create_line[60],keep_going = TRUE;
	short boat_there,horse_there,spec_num;
	unsigned char ter;
	Boolean check_f = FALSE;
		
	if (debug_on == TRUE)
		forced = TRUE;
	
	// remove if not registered
	/*
	if ((scenario.out_width != 3) || (scenario.out_height != 3) ||
		(scenario.num_towns != 21) || (scenario.town_size[3] != 1) || (scenario.town_size[9] != 0)) {
			ASB("Blades of Exile must be registered");
			ASB("before you can play scenarios besides");
			ASB("the unmodified Valley of Dying things.");
			print_buf();
			return FALSE;
			}
	*/
			
	if (monst_there(destination) > T_M)
		keep_going = check_special_terrain(destination,1,0,&spec_num,&check_f);
	if (check_f == TRUE)
		forced = TRUE;

	if (spec_num == 50)
		forced = TRUE;
	ter = t_d.terrain[destination.x][destination.y];
	
	if (keep_going == TRUE) {
		if (party.in_boat >= 0) {
				if ((is_blocked(destination) == FALSE) && (is_special(destination) == FALSE)
				// If to bridge, exit if heading diagonal, keep going is head horiz or vert
		&& ( (scenario.ter_types[ter].boat_over == FALSE)
		|| ((destination.x != c_town.p_loc.x) && (destination.y != c_town.p_loc.y)))) {
						add_string_to_buf("You leave the boat.             ");
						party.in_boat = -1;
						}
				else if ((destination.x != c_town.p_loc.x) && (destination.y != c_town.p_loc.y))
					return FALSE;	
				// Crossing bridge: land or go through
				else if ((is_blocked(destination) == FALSE) && (scenario.ter_types[ter].boat_over == TRUE)) {
					if ((fancy_choice_dialog(1086,0)) == 1)
						forced = TRUE;
						else if (is_blocked(destination) == FALSE) {
							add_string_to_buf("You leave the boat.             ");
							party.in_boat = -1;					
							}				
					}
				// boat in destination
				else if (town_boat_there(destination) < 30) {
					add_string_to_buf("  Boat there already.             ");
					return FALSE;
					}
				// water or lava
				else if (scenario.ter_types[ter].boat_over == TRUE)
					forced = TRUE;
			}

		if (((boat_there = town_boat_there(destination)) < 30) && (party.in_boat < 0)) {
			if (party.boats[boat_there].property == TRUE) {
				add_string_to_buf("  Not your boat.             ");
				return FALSE;			
				}
			give_help(61,0,0);
			add_string_to_buf("Move: You board the boat.           ");
			party.in_boat = boat_there;
			party.direction = set_direction(c_town.p_loc, destination); 
		
			c_town.p_loc = destination;
			center = c_town.p_loc;

			return TRUE;
		} 
		else if (((horse_there = town_horse_there(destination)) < 30) && (party.in_horse < 0)) {
			if (party.horses[horse_there].property == TRUE) {
				add_string_to_buf("  Not your horses.             ");
				return FALSE;			
				}
			give_help(60,0,0);
			add_string_to_buf("Move: You mount the horses.           ");
			play_sound(84);
			party.in_horse = horse_there;
			party.direction = set_direction(c_town.p_loc, destination); 
		
			c_town.p_loc = destination;
			center = c_town.p_loc;

			return TRUE;
		} 
		else if ((is_blocked(destination) == FALSE) || (forced == 1)) {
			if (party.in_horse >= 0) {
				if ((scenario.ter_types[ter].special >= 2) && (scenario.ter_types[ter].special <= 4)) {
					ASB("Your horses quite sensibly refuse.");
					return FALSE;
					}
				if (scenario.ter_types[ter].block_horse == TRUE) {
					ASB("You can't take horses there!");
					return FALSE;
					}
				if ((c_town.town.lighting > 0) && (get_ran(1,0,1) == 0)) {
					ASB("The darkness spooks your horses.");
					return FALSE;
					}
			
				}
			party.direction = set_direction(c_town.p_loc, destination);
			c_town.p_loc = destination;
			sprintf ((char *) create_line, "Moved: %s",dir_string[party.direction]);
			add_string_to_buf((char *) create_line);
//			place_treasure(destination,5,3);

			move_sound(t_d.terrain[destination.x][destination.y],(short) party.age);
			
			if (party.in_boat >= 0) {
				// Waterfall!!!
				while (scenario.ter_types[t_d.terrain[destination.x][destination.y + 1]].special == 15) {
					add_string_to_buf("  Waterfall!                     ");
					destination.y += 2;
					c_town.p_loc.y += 2;
					update_explored(party.p_loc);
					initiate_redraw();
					print_buf();
					if ((cave_lore_present() > 0) && (get_ran(1,0,1) == 0))
						add_string_to_buf("  (No supplies lost.)");
					else if (party.food > 1800){
						add_string_to_buf("  (Many supplies lost.)");
						party.food -= 50;
					}
					else {
						int n = party.food;
						char s[25];
						party.food = (party.food * 19) / 20;
						sprintf(s,"  (%d supplies lost.)",n - party.food);
						add_string_to_buf(s);
					}
					put_pc_screen();
					play_sound(28);
					pause(8);
				}
				party.boats[party.in_boat].loc = c_town.p_loc;
				party.boats[party.in_boat].which_town = c_town.town_num;
			}
			if (party.in_horse >= 0) {
					party.horses[party.in_horse].loc = c_town.p_loc;
					party.horses[party.in_horse].which_town = c_town.town_num;
				}
			center = c_town.p_loc;
			return TRUE;
			}
		else {
			if (is_door(destination) == TRUE)
				sprintf ((char *) create_line, "Door locked: %s               ",dir_string[set_direction(c_town.p_loc, destination)]);		
				else sprintf ((char *) create_line, "Blocked: %s               ",dir_string[set_direction(c_town.p_loc, destination)]);		
			add_string_to_buf((char *) create_line);
			return FALSE;
			}
		}
	return FALSE;
}



Boolean someone_poisoned()
{
	short i;
	
	for (i = 0; i < 6; i++)
		if ((adven[i].main_status == 1) && (adven[i].status[2] > 0))
			return TRUE;
	return FALSE;
}

short nearest_monster()
{
	short i = 100,j,s;

	for (j = 0; j < 10; j++)
		if (party.out_c[j].exists == TRUE) {
			s = dist(party.p_loc,party.out_c[j].m_loc);
			i = min(i,s);
		}
	return i;
}

void setup_outdoors(location where)
{
	update_explored(where);
}

short get_outdoor_num()////
{
	return (scenario.out_width * (party.outdoor_corner.y + party.i_w_c.y) + party.outdoor_corner.x + party.i_w_c.x);
}

short count_walls(location loc)
{
	unsigned char walls[31] = {5,6,7,8,9, 10,11,12,13,14, 15,16,17,18,19, 20,21,22,23,24,
							25,26,27,28,29, 30,31,32,33,34, 35};
	short answer = 0;
	short k = 0;

	for (k = 0; k < 31 ; k++) {
		if (out[loc.x + 1][loc.y] == walls[k]) 
			answer++;	
		if (out[loc.x - 1][loc.y] == walls[k]) 
			answer++;	
		if (out[loc.x][loc.y + 1] == walls[k]) 
			answer++;	
		if (out[loc.x][loc.y - 1] == walls[k]) 
			answer++;	
		}
	return answer;		
}

Boolean is_sign(unsigned char ter)
{
	
	if (scenario.ter_types[ter].special == 11)
		return TRUE;
	return FALSE;
}