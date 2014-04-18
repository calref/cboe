
#include <cmath>

//#include "item.h"

#include "boe.global.h"

#include "classes.h"
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
#include "fileio.h"
#include "dlogutil.h"
#include "dialog.h"
#include "scrollbar.h"
#include "boe.menus.h"
#include "winutil.h"

RECT bottom_buttons[7];
RECT town_buttons[10];
RECT combat_buttons[9];
RECT world_screen = {23, 23, 346, 274};
// TODO: The duplication of RECT here shouldn't be necessary...
RECT item_screen_button_rects[9] = {
	RECT{125,10,141,28},RECT{125,40,141,58},RECT{125,68,141,86},RECT{125,98,141,116},RECT{125,126,141,144},RECT{125,156,141,174},
	RECT{126,176,141,211},
	RECT{126,213,141,248},
	RECT{127,251,140,267}};

RECT border_rect[4] = {RECT{5, 5, 15, 283}, RECT{5, 5, 355, 15},
	RECT{345, 5, 355, 283}, RECT{5, 273, 355, 283}};
RECT medium_buttons[4] = {RECT{383,190,401,225}, RECT{402, 190, 420, 225},
	RECT{383, 227, 401, 263}, RECT{402, 227,420, 263}}; ;

RECT item_buttons[8][6];
// name, use, give, drip, info, sell/id
RECT pc_buttons[6][5];
// name, hp, sp, info, trade
short num_chirps_played = 0;

extern RECT startup_button[6];
extern bool flushingInput;
bool ghost_mode;
RECT startup_top;

// For menu spell casting, some info needs to be stored up here.
short refer_mage[62] = {
	0,2,1,1,2,2,0,2,2,0, 2,2,2,2,1,2,2,2,2,2, 0,1,2,0,2,2,3,3,2,1,
	2,2,1,0,2,2,3,2, 0,1,2,0,2,3,2,3, 2,1,2,3,2,2,2,0, 1,1,1,0,3,2,2,3};
short refer_priest[62] = {
	1,0,0,2,0,0,0,0,0,2, 1,0,2,0,2,2,0,2,3,0, 0,0,2,0,0,0,2,0,0,3,
	0,1,2,0,3,0,0,0, 1,0,0,2,0,3,0,2, 0,0,0,0,2,1,1,1, 0,2,0,2,1,2,0,0};
// 0 - refer  1 - do in combat immed.  2 - need targeting  3 - need fancy targeting
short mage_need_select[62] = {
	0,0,1,1,0,0,0,0,0,0, 0,0,0,0,1,0,0,0,0,0, 0,1,0,0,0,0,0,0,0,1,
	0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,1,0,0,0,0};
short priest_need_select[62] = {
	1,1,1,0,0,1,1,0,0,0, 1,1,0,0,0,0,0,0,0,1, 1,0,0,0,1,0,0,1,1,0,
	0,0,0,1,0,1,1,0, 0,1,2,0,0,0,0,0, 0,1,0,2,0,0,0,0, 0,0,2,0,0,0,0,0};
// 0 - no select  1 - active only  2 - any existing

// TODO: The duplication of RECT here shouldn't be necessary...
word_rect_type preset_words[9] = {
	{"Look",RECT{366,4,386,54}},{"Name",RECT{366,70,386,130}},{"Job",RECT{366,136,386,186}},
	{"Buy",RECT{389,4,409,54}},{"Sell",RECT{389,70,409,120}},{"Record",RECT{389,121,409,186}},
	{"Done",RECT{389,210,409,270}},{"Go Back",RECT{366,190,386,270}},
	{"Ask About...",RECT{343,4,363,134}}};
cItemRec start_items[6] = {cItemRec('nife'),cItemRec('buck'),cItemRec('bow '),cItemRec('arrw'),cItemRec('pole'),cItemRec('helm')};
bool item_area_button_active[8][6];
bool pc_area_button_active[6][5];
short item_bottom_button_active[9] = {0,0,0,0,0, 0,1,1,1};
bool unreg_party_in_scen_not_check = false;

RECT pc_help_button,pc_area_rect,item_area_rect;

short current_terrain_type = 0,num_out_moves = 0;
short door_pc,store_drop_item;
short current_switch = 6;
cOutdoors::cWandering store_wandering_special;
long dummy;
short store_shop_type;

short store_selling_values[8] = {0,0,0,0,0,0,0,0};

extern short cen_x, cen_y, stat_window,give_delays;//,pc_moves[6];
extern eGameMode overall_mode;
extern location	to_create;
extern bool All_Done,play_sounds,frills_on,spell_forced,save_maps,monsters_going;
extern bool party_in_memory,in_scen_debug;

// game info globals

//extern stored_town_maps_type town_maps;
extern sf::RenderWindow mainPtr;
////extern party_record_type	party;
//extern cOutdoors outdoors[2][2];
//extern current_town_type	c_town;
//extern big_tr_type t_d;
//extern unsigned char out[96][96],out_e[96][96];
extern short which_item_page[6];
extern short /*town_size[3],*/store_spell_target,pc_last_cast[2][6],pc_casting,store_mage,store_priest;
//extern town_item_list	t_i; // shouldn't be here
//extern unsigned char misc_i[64][64];
extern short spec_item_array[60];
extern cScenario scenario;
extern cUniverse univ;
//extern piles_of_stuff_dumping_type *data_store;

// combat globals


extern short which_combat_type,num_targets_left;
extern ter_num_t combat_terrain[64][64];
extern location center;
extern location pc_pos[6];
extern short current_pc,town_type;
extern short combat_active_pc,stat_screen_mode;

extern bool map_visible,diff_depth_ok;
extern sf::RenderWindow mini_map;

//extern stored_items_list_type stored_items[3];
//extern stored_outdoor_maps_type o_maps;
extern location ul;
extern std::shared_ptr<cScrollbar> text_sbar,item_sbar,shop_sbar;
extern short shop_identify_cost;


const char *dir_string[] = {"North", "NorthEast", "East", "SouthEast", "South", "SouthWest", "West", "NorthWest"};
char get_new_terrain();
cCreature save_monster_type;

short wand_loc_count = 0;
short monst_place_count = 0; // 1 - standard place	2 - place last

// 0 - whole area, 1 - active area 2 - graphic 3 - item name
// 4 - item cost 5 - item extra str  6 - item help button
RECT shopping_rects[8][7];
pending_special_type special_queue[20];
bool end_scenario = false;

void init_screen_locs() ////
{
	short i,j,k,l;
	RECT startup_base = {279,5,327,306};
	RECT shop_base = {63,12,99,267};
	
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
			shopping_rects[i][j].offset(0,i * 36);
		}
	
	
	for (i = 0; i < 6; i++) {
		startup_button[i] = startup_base;
		startup_button[i].offset(301 * (i / 3)  - 18,48 * (i % 3));
	}
	startup_top.top = 5;
	startup_top.bottom = startup_button[STARTBTN_LOAD].top;
	startup_top.left = 5;
	startup_top.right = startup_button[STARTBTN_JOIN].right;
	
	for (i = 0; i < 200; i++)
		for (j = 0; j < 8; j++)
			for (k = 0; k < 64; k++)////
				univ.town_maps[i][j][k] = 0;
	
	for (i = 0; i < 100; i++)
		for (k = 0; k < 6; k++)
			for (l = 0; l < 48; l++)
				univ.out_maps[i][k][l] = 0;
	
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
			item_buttons[i][j].offset(0,13 * i);
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
			pc_buttons[i][j].offset(0,13 * i);
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

bool prime_time()
{
	if ((overall_mode < MODE_TALK_TOWN) || (overall_mode == MODE_COMBAT))
		return true;
	return false;
}

bool handle_action(sf::Event event)
{
	short i,j,k,item_hit,which_t,s1,s2,s3;
	bool are_done = false;
	bool need_redraw = false, did_something = false, need_reprint = false;
	bool town_move_done = false,pc_delayed = false;
	short storage;
	location destination,cur_loc,loc_in_sec,cur_direction;
	unsigned char debug_storage;
	short find_direction_from,ter_looked_at,button_hit = 12,store_cur_pc;
	short store_sp[6];
	ter_num_t ter;
	
	char str[60];
	location the_point,point_in_area;
	
	
	debug_storage = univ.party.spec_items[1];
	
	the_point = location(event.mouseButton.x, event.mouseButton.y);
	the_point.x -= ul.x;
	the_point.y -= ul.y;
	for (i = 0; i < 20; i++) // TODO: Does this cause problems by leaving some specials uncalled?
		special_queue[i].spec = -1;
	end_scenario = false;
	
	if (unreg_party_in_scen_not_check == true) {
		if (/*enter_password() == */false) {
			end_scenario = true;
		}
		else {
			in_scen_debug = true;
			ASB("Debug mode ON.");
		}
		unreg_party_in_scen_not_check = false;
	}
	// Now split off the extra stuff, like talking and shopping.
	if (overall_mode == MODE_TALKING) {
		handle_talk_event(the_point);
		if (overall_mode != MODE_TALKING)
			return false;
	}
	if (overall_mode == MODE_SHOPPING) {
		handle_shop_event(the_point);
		if (overall_mode != MODE_SHOPPING)
			return false;
	}
	
	num_chirps_played = 0;
	
	// MARK: First, figure out where party is
	switch (overall_mode) {
		case MODE_OUTDOORS: case MODE_LOOK_OUTDOORS:
			cur_loc = univ.party.p_loc;
			for (i = 0; i < 7; i++)
				if(the_point.in(bottom_buttons[i])) {
					button_hit = i;
					if (spell_forced == false)
						main_button_click(overall_mode,bottom_buttons[i]);
				}
			break;
			
		case MODE_TOWN: case MODE_TALK_TOWN: case MODE_USE_TOWN: case MODE_LOOK_TOWN: case MODE_DROP_TOWN:
			// I think 5 is "town drop"
//			cur_loc = c_town.p_loc;
			cur_loc = center;
			for (i = 0; i < 8; i++)
				if(the_point.in(town_buttons[i])) {
					button_hit = i;
					if (spell_forced == false)
						main_button_click(overall_mode,town_buttons[i]);
				}
			break;
			
		case MODE_TALKING: case MODE_SHOPPING: break;
			
		case MODE_TOWN_TARGET: case MODE_COMBAT: case MODE_SPELL_TARGET: case MODE_FIRING: case MODE_THROWING:
		case MODE_FANCY_TARGET: case MODE_DROP_COMBAT: case MODE_LOOK_COMBAT:
			cur_loc = (overall_mode > MODE_COMBAT) ? center : pc_pos[current_pc];
			for (i = 0; i < 9; i++)
				if(the_point.in(combat_buttons[i])) {
					button_hit = i;
					if (spell_forced == false)
						main_button_click(overall_mode,combat_buttons[i]);
				}
			break;
		default:
			// either resting or startup; do nothing
			// TODO: A call to handle_startup_action() would make sense here, though
			break;
	}
	
	// MARK: Then, handle a button being hit.
	if (button_hit != 12)
		switch(button_hit) {
			case 0: case 1:
				if (someone_awake() == false) {
					ASB("Everyone's asleep/paralyzed.");
					need_reprint = true;
					need_redraw = true;
				}
				if (overall_mode == MODE_OUTDOORS) {
					cast_spell(button_hit,0);
					spell_forced = false;
					need_reprint = true;
					need_redraw = true;
				}
				else if (overall_mode == MODE_TOWN) {
					for (i = 0; i < 6; i++)
						store_sp[i] = univ.party[i].cur_sp;
					cast_spell(button_hit,1);
					spell_forced = false;
					need_reprint = true;
					need_redraw = true;
					for (i = 0; i < 6; i++)
						if (store_sp[i] != univ.party[i].cur_sp)
							did_something = true;
				}
				else if (overall_mode == MODE_TOWN_TARGET) {
					add_string_to_buf("  Cancelled.                   ");
					overall_mode = MODE_TOWN;
				}
				else if (overall_mode >= MODE_COMBAT) {
					if ((overall_mode == MODE_COMBAT) && (button_hit == 0)) {
						did_something = combat_cast_mage_spell();
						need_reprint = true;
					}
					if ((overall_mode == MODE_COMBAT) && (button_hit == 1)) {
						did_something = combat_cast_priest_spell();
						need_reprint = true;
					}
					if ((overall_mode != MODE_SPELL_TARGET) && (overall_mode != MODE_FANCY_TARGET)) {
						need_redraw = true;
					}
					else did_something = false;
					spell_forced = false;
					redraw_terrain();
				}
				else if ((overall_mode == MODE_SPELL_TARGET) || (overall_mode == MODE_FANCY_TARGET)) {
					add_string_to_buf("  Cancelled.         ");
					overall_mode = MODE_COMBAT;
					center = pc_pos[current_pc];
					pause(10);
					need_redraw = true;
				}
				put_pc_screen();
				put_item_screen(stat_window,0);
				break;
				
			case 2:
				if (overall_mode == MODE_OUTDOORS) {
					overall_mode = MODE_LOOK_OUTDOORS;
					add_string_to_buf("Look: Select a space. Option-click        ");
					add_string_to_buf("  to look more than once.");
					need_redraw = true;
				}
				if (overall_mode == MODE_TOWN) {
					overall_mode = MODE_LOOK_TOWN;
					add_string_to_buf("Look: Select a space. Option-click        ");
					add_string_to_buf("  to look more than once.");
					need_redraw = true;
				}
				if (overall_mode == MODE_COMBAT) {
					overall_mode = MODE_LOOK_COMBAT;
					add_string_to_buf("Look: Select a space. Option-click        ");
					add_string_to_buf("  to look more than once.");
					need_redraw = true;
				}
				break;
				
			case 3:
				if (overall_mode == MODE_COMBAT) {
					add_string_to_buf("Parry. ");
					char_parry();
					did_something = true;
					need_reprint = true;
					need_redraw = true;
				}
				if (overall_mode == MODE_TOWN) {
					overall_mode = MODE_TALK_TOWN;
					add_string_to_buf("Talk: Select someone    ");
					need_reprint = true;
				}
				if (overall_mode == MODE_OUTDOORS) { // Resting
					i = 0;
					ter = univ.out[univ.party.p_loc.x][univ.party.p_loc.y];
					if (univ.party.in_boat >= 0)////
						add_string_to_buf("Rest:  Not in boat.               ");
					else if (someone_poisoned() == true)
						add_string_to_buf("Rest: Someone poisoned.           ");
					else if (univ.party.food <= 12)
						add_string_to_buf("Rest: Not enough food.            ");
					else if (nearest_monster() <= 3)
						add_string_to_buf("Rest: Monster too close.            ");
					else if ((scenario.ter_types[ter].special == TER_SPEC_DAMAGING) || (scenario.ter_types[ter].special == TER_SPEC_DANGEROUS))
						add_string_to_buf("Rest: It's dangerous here.");////
					else if (flying() == true)
						add_string_to_buf("Rest: Not while flying.           ");
					else {
						add_string_to_buf("Resting...                    ");
						print_buf();
						play_sound(20);
						draw_rest_screen();
						pause(25);
						univ.party.food -= 6;
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
						univ.party.age += 1200;////
						add_string_to_buf("  Rest successful.                ");
						k = get_ran(5,1,10);
						heal_party(k);
						restore_sp_party(50);
						put_pc_screen();
					}
					need_reprint = true;
					need_redraw = true;
				}
				break;
				
			case 4:
				if (overall_mode == MODE_OUTDOORS) {
					give_help(62,0);
					display_map();
					make_cursor_sword();
				}
				if ((overall_mode == MODE_TOWN) || (overall_mode == MODE_COMBAT)) {
					if (univ.party.in_boat >= 0) {////
						j = 0;
						add_string_to_buf("Get: Not while in boat.           ");
					}
					if (overall_mode == MODE_TOWN)
						j = get_item(univ.town.p_loc,6,false);
					else {
						j = get_item(pc_pos[current_pc],current_pc,false);
						take_ap(4);
					}
					if (j > 0) {
						put_item_screen(stat_window, 0);
						put_pc_screen();
						need_redraw = true;
						did_something = true;
					}
					need_reprint = true;
				}
				break;
				
			case 5:
				if (overall_mode == MODE_OUTDOORS) {
					save_party(univ.file);
					need_redraw = true;
					current_switch = 6;
					break;
				}
				if (overall_mode == MODE_TOWN) {
					add_string_to_buf("Use: Select a space or item.");
					add_string_to_buf("  (Hit button again to cancel.)");
					need_reprint = true;
					overall_mode = MODE_USE_TOWN;
				}
				else if (overall_mode == MODE_USE_TOWN) {
					overall_mode = MODE_TOWN;
					need_reprint = true;
					add_string_to_buf("  Cancelled.");
				}
				if (overall_mode == MODE_COMBAT) {
					need_reprint = true;
					need_redraw = true;
					pc_delayed = true;
				}
				
				break;
				
			case 6:
				if (overall_mode == MODE_OUTDOORS) {
					do_load();
					break;
				}
				if (overall_mode == MODE_TOWN) {
					give_help(62,0);
					display_map();
					make_cursor_sword();
				}
				if (overall_mode == MODE_COMBAT) {
					load_missile();
					need_reprint = true;
					redraw_terrain();
				}
				else if ((overall_mode == MODE_FIRING) || (overall_mode == MODE_THROWING)) {
					add_string_to_buf("  Cancelled.             ");
					center = pc_pos[current_pc];
					pause(10);
					need_redraw = true;
					overall_mode = MODE_COMBAT;
				}
				break;
				
			case 7:
				if (overall_mode == MODE_TOWN) {
					if (univ.party.in_boat >= 0) {////
						need_reprint = true;
						add_string_to_buf("Combat: Not while in boat.           ");
					}
					else if (univ.party.in_horse >= 0) {
						need_reprint = true;
						add_string_to_buf("Combat: Not while on horseback.           ");
					}
					else {
						add_string_to_buf("Combat!                 ");
						play_sound(18);
						need_reprint = true;
						start_town_combat(univ.party.direction);
						need_redraw = true;
						current_pc = 6;
						did_something = true;
						put_pc_screen();
					}
				}
				else if (overall_mode == MODE_COMBAT) {
					if (which_combat_type == 0) {
						if (hit_end_c_button() == true) {
							end_town_mode(0,univ.town.p_loc);
							play_sound(93);
							add_string_to_buf("End combat.               ");
							handle_wandering_specials(0,1);
							menu_activate();
							put_pc_screen();
							set_stat_window(current_pc);
						}
						else add_string_to_buf("Can't end combat yet.        ");
					}
					else {
						univ.party.direction = end_town_combat();
						center = univ.town.p_loc;
						//put_pc_screen();
						set_stat_window(current_pc);
						redraw_screen(REFRESH_TERRAIN | REFRESH_TEXT);
						play_sound(93);
						need_reprint = true;
						need_redraw = true;
						did_something = true;
						menu_activate();
					}
					need_redraw = true;
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
					need_reprint = true;
				}
				break;
		}
	
/*	if (overall_mode == 30) /// I don't know what this is for.
		if (PtInRect(the_point, &text_panel_rect)) {
			through_sending();
			restore_mode();
			print_buf();
		} */
	
	// MARK: Begin : click in terrain
	if ((the_point.in(world_screen)) && ((is_out()) || (is_town()) || (is_combat())) ){
		i = (the_point.x - 23) / 28;
		j = (the_point.y - 23) / 36;
		
		destination = cur_loc;
		
		if ((overall_mode == MODE_OUTDOORS) || (overall_mode == MODE_TOWN) || (overall_mode == MODE_COMBAT))
			if ((i == 4) & (j == 4)) { // Pausing
				if (overall_mode == MODE_COMBAT) {
					char_stand_ready();
					add_string_to_buf("Stand ready.  ");
					if (univ.party[current_pc].status[6] > 0) {
						add_string_to_buf("You clean webs.  ");
						univ.party[current_pc].status[6] = move_to_zero(univ.party[current_pc].status[6]);
						univ.party[current_pc].status[6] = move_to_zero(univ.party[current_pc].status[6]);
						put_pc_screen();
					}
					check_fields(pc_pos[current_pc],2,current_pc);
				}
				else {
					add_string_to_buf("Pause.");
					for (k = 0; k < 6; k++)
						if ((univ.party[k].main_status == 1) && (univ.party[k].status[6] > 0)) {
							sprintf((char *) str,"%s cleans webs.",univ.party[k].name.c_str());
							add_string_to_buf((char *) str);
							univ.party[k].status[6] = move_to_zero(univ.party[k].status[6]);
							univ.party[k].status[6] = move_to_zero(univ.party[k].status[6]);
						}
					if (univ.party.in_horse >= 0) {
						if (overall_mode == MODE_OUTDOORS) {
							univ.party.horses[univ.party.in_horse].which_town = 200;
							univ.party.horses[univ.party.in_horse].loc_in_sec = global_to_local(univ.party.p_loc);
							univ.party.horses[univ.party.in_horse].loc = univ.party.p_loc;
							univ.party.horses[univ.party.in_horse].sector.x = univ.party.outdoor_corner.x + univ.party.i_w_c.x;
							univ.party.horses[univ.party.in_horse].sector.y = univ.party.outdoor_corner.y + univ.party.i_w_c.y;
							univ.party.in_horse = -1;
						}
						else if (overall_mode == MODE_TOWN){
							univ.party.horses[univ.party.in_horse].loc = univ.town.p_loc;
							univ.party.horses[univ.party.in_horse].which_town = univ.town.num;
							univ.party.in_horse = -1;
						}
					}
					put_pc_screen();
					check_fields(univ.town.p_loc,1,0);
				}
				
				did_something = true;
				need_redraw = true;
			}
			else {
				cur_direction = get_cur_direction(the_point);
				destination.x += cur_direction.x;
				destination.y += cur_direction.y;
				
//				for (i = 0; i < 6; i++) // debug
//					if (univ.party[i].exp_adj != 100) {
//						ASB("Error: Flag 1");
//					}
				
				// MARK: Begin: Moving
				if (overall_mode == MODE_COMBAT) {
					if (pc_combat_move(destination) == true) {
						center = pc_pos[current_pc];
						did_something = true;
						update_explored(destination);
					}
					need_redraw = true;
					menu_activate();
				}
				if (overall_mode == MODE_TOWN) {
					if (someone_awake() == false) {
						ASB("Everyone's asleep/paralyzed.");
						need_reprint = true;
						need_redraw = true;
					}
					else {
						need_redraw = true;
						if (town_move_party(destination,0) == true) {
							did_something = true;
							center = univ.town.p_loc;
							update_explored(destination);
							
							if (loc_off_act_area(univ.town.p_loc) == true) {
								
								destination = end_town_mode(0,destination);
								
								
								town_move_done = true;
								flushingInput = true;
							}
						}
						else need_reprint = true;
						menu_activate();
					}
				}
				if (overall_mode == MODE_OUTDOORS)  {
					if (outd_move_party(destination,town_move_done) == true) {
						center = destination;
						need_redraw = true;
						did_something = true;
						update_explored(univ.party.p_loc);
						
						menu_activate();
					}
					else need_redraw = true;
					
					storage = univ.out[univ.party.p_loc.x][univ.party.p_loc.y];
					if (scenario.ter_types[storage].special == TER_SPEC_TOWN_ENTRANCE) {//// town entry
						
						if (univ.party.direction == 0) find_direction_from = 2;
						else if (univ.party.direction == 4) find_direction_from = 0;
						else if (univ.party.direction < 4) find_direction_from = 3;
						else find_direction_from = 1;
						
						for (i = 0; i < 8; i++)
							if (univ.party.loc_in_sec == univ.out.outdoors[univ.party.i_w_c.x][univ.party.i_w_c.y].exit_locs[i]) {
								which_t = univ.out.outdoors[univ.party.i_w_c.x][univ.party.i_w_c.y].exit_dests[i];
								if (which_t >= 0)
									start_town_mode(univ.out.outdoors[univ.party.i_w_c.x][univ.party.i_w_c.y].exit_dests[i], find_direction_from);
								if (is_town() == true) {
									need_redraw = false;
									i = 8;
									if (univ.party.in_boat >= 0)
										univ.party.boats[univ.party.in_boat].which_town = univ.town.num;
									if (univ.party.in_horse >= 0)
										univ.party.horses[univ.party.in_horse].which_town = univ.town.num;
								}
							}
					}
				} // End if(overall_mode == MODE_OUTDOORS)
				
			}
		// MARK: End: Moving
		
		// MARK: Begin: Looking at something
		if ((overall_mode == MODE_LOOK_OUTDOORS) || (overall_mode == MODE_LOOK_TOWN) || (overall_mode == MODE_LOOK_COMBAT)) {
			destination.x = destination.x + i - 4;
			destination.y = destination.y + j - 4;
			
			need_reprint = true;
			
//			if ((can_see(cur_loc,destination) >= 4) || ((overall_mode != 35) && (loc_off_world(destination) == true)))
			if ((overall_mode != MODE_LOOK_COMBAT) && (party_can_see(destination) == 6))
				add_string_to_buf("  Can't see space.         ");
			else if ((overall_mode == MODE_LOOK_COMBAT) && (can_see(pc_pos[current_pc],destination,0) >= 4))
				add_string_to_buf("  Can't see space.         ");
			else {
				add_string_to_buf("You see...               ");
				ter_looked_at = do_look(destination);
				if ((overall_mode == MODE_LOOK_TOWN) || (overall_mode == MODE_LOOK_COMBAT))
					if (adjacent(univ.town.p_loc,destination) == true)
						if (adj_town_look(destination) == true)
							need_redraw = true;
				if (is_sign(ter_looked_at)) {
					print_buf();
					need_reprint = false;
					k = 0;
					if (overall_mode == MODE_LOOK_TOWN) {
						while (k < 15) {
							if (destination == univ.town->sign_locs[k]) {
								need_reprint = true;
								if (adjacent(univ.town->sign_locs[k],univ.town.p_loc)==true)
									do_sign(univ.town.num,k,(short) ter_looked_at,destination);
								else add_string_to_buf("  Too far away to read sign.      ");
							}
							k++;
						}
					}
					if (overall_mode == MODE_LOOK_OUTDOORS) {
						loc_in_sec = univ.party.loc_in_sec;
						loc_in_sec.x += i - 4;
						loc_in_sec.y += j - 4;
						while (k < 8) {
							if (loc_in_sec == univ.out.outdoors[univ.party.i_w_c.x][univ.party.i_w_c.y].sign_locs[k]) {
								need_reprint = true;
								if (adjacent(univ.out.outdoors[univ.party.i_w_c.x][univ.party.i_w_c.y].sign_locs[k],univ.party.loc_in_sec)==true)
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
			if(!sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt) && !sf::Keyboard::isKeyPressed(sf::Keyboard::RAlt)) {
				if (overall_mode == MODE_LOOK_COMBAT) {
					overall_mode = MODE_COMBAT;
					center = pc_pos[current_pc];
					pause(5);
					need_redraw = true;
				}
				else if (overall_mode == MODE_LOOK_TOWN) {
					overall_mode = MODE_TOWN;
					center = univ.town.p_loc;
					need_redraw = true;
				}
				else if (overall_mode == MODE_LOOK_OUTDOORS)
					overall_mode = MODE_OUTDOORS;
				
			}
		}
		
		// MARK: End: looking at something
		
		// MARK: Begin : talking to someone
		if (overall_mode == MODE_TALK_TOWN) {
			destination.x = destination.x + i - 4;
			destination.y = destination.y + j - 4;
			
			if ((can_see(cur_loc,destination,0) >= 4) || (loc_off_world(destination) == true)) {
				add_string_to_buf("  Can't see space              ");
				need_reprint = true;
			}
			else {
				
				for (i = 0; i < univ.town->max_monst(); i++) {
					if (monst_on_space(destination,i) == true) {
						did_something = true;
						need_redraw = true;
						if (univ.town.monst[i].attitude % 2 == 1) {
							add_string_to_buf("  Creature is hostile.        ");
						}
						else if ((univ.town.monst[i].summoned > 0)////
								 || (univ.town.monst[i].personality < 0))
							add_string_to_buf("Talk: No response.            ");
						else {
							start_talk_mode(i,univ.town.monst[i].personality,univ.town.monst[i].number,
											univ.town.monst[i].facial_pic);
							did_something = false;
							need_redraw = false;
							break;
						}
					}
				}
				if ((did_something == false) && (overall_mode != MODE_TALKING)){
					add_string_to_buf("  Nobody there");
					need_reprint = true;
				}
				if (overall_mode != MODE_TALKING)
					overall_mode = MODE_TOWN;
			}
			if (overall_mode != MODE_TALKING)
				need_redraw = true;
		}
		// MARK: End : talking to someone
		
		// MARK: Begin : Targeting a space
		if ((overall_mode == MODE_SPELL_TARGET) || (overall_mode == MODE_FIRING) || (overall_mode == MODE_THROWING) ||
			(overall_mode == MODE_FANCY_TARGET) || (overall_mode == MODE_DROP_COMBAT)) {
			destination.x = destination.x + i - 4;
			destination.y = destination.y + j - 4;
			if (overall_mode == MODE_SPELL_TARGET)
				do_combat_cast(destination);
			if ((overall_mode == MODE_THROWING) || (overall_mode == MODE_FIRING))
				fire_missile(destination);
			if (overall_mode == MODE_FANCY_TARGET) {
				place_target(destination);
				need_reprint = true;
			}
			if (overall_mode != MODE_FANCY_TARGET) {
				did_something = true;
				center = pc_pos[current_pc];
			}
			if (overall_mode == MODE_DROP_COMBAT) { // dropping
				if (adjacent(pc_pos[current_pc],destination) == false)
					add_string_to_buf("Drop: must be adjacent.");
				else {
					drop_item(current_pc,store_drop_item,destination);
					take_ap(1);
				}
			}
			pause(6);
			need_redraw = true;
			if ((overall_mode >= MODE_COMBAT) && (overall_mode < MODE_TALKING) && (overall_mode != MODE_FANCY_TARGET))
				overall_mode = MODE_COMBAT;
			put_pc_screen();
			put_item_screen(stat_window,0);
		}
		if ((overall_mode > MODE_TALK_TOWN) && (overall_mode < MODE_COMBAT/*6*/)) {
			destination.x = destination.x + i - 4;
			destination.y = destination.y + j - 4;
			if (overall_mode == MODE_TOWN_TARGET) {
				cast_town_spell(destination);
				did_something = true;
			}else if(overall_mode == MODE_USE_TOWN) {
				if (adjacent(destination,univ.town.p_loc) == false)
					add_string_to_buf("  Must be adjacent.              ");
				else
					did_something = use_space(destination);
			}else if(overall_mode == MODE_DROP_TOWN) {
				if (adjacent(univ.town.p_loc,destination) == false)
					add_string_to_buf("Drop: must be adjacent.");
				else if (get_obscurity(destination.x,destination.y) == 5)
					ASB("Drop: Space is blocked.");
				else drop_item(current_pc,store_drop_item,destination);
			}
			overall_mode = MODE_TOWN;
			need_redraw = true;
			put_pc_screen();
			put_item_screen(stat_window,0);
		}
		// MARK: End : Targeting a space
		
	}
	// MARK: End: click in terrain
	
	// MARK: Begin: Screen shift
	if ((overall_mode == MODE_SPELL_TARGET) ||  (overall_mode == MODE_FIRING) || (overall_mode == MODE_THROWING) || (overall_mode == MODE_FANCY_TARGET)
		|| (overall_mode == MODE_LOOK_COMBAT) || (overall_mode == MODE_LOOK_TOWN)) {
		if ((the_point.in(border_rect[0])) && (center.y > univ.town->in_town_rect.top)
			&& (center.y > 4)) {
			center.y--;
			need_redraw = true;
		}
		if ((the_point.in(border_rect[1])) && (center.x > univ.town->in_town_rect.left)
			&& (center.x > 4)) {
			center.x--;
			need_redraw = true;
		}
		if ((the_point.in(border_rect[2])) && (center.y < univ.town->in_town_rect.bottom)
			&& (center.y < univ.town->max_dim() - 5)) {
			center.y++;
			need_redraw = true;
		}
		if ((the_point.in(border_rect[3])) && (center.x < univ.town->in_town_rect.right)
			&& (center.x < univ.town->max_dim() - 5)) {
			center.x++;
			need_redraw = true;
		}
	}
	// MARK: End: Screen shift
	
	
	// MARK: Process clicks in PC stats area
	if (the_point.in(pc_area_rect)) {
		point_in_area = the_point;
		point_in_area.x -= PC_WIN_UL_X;
		point_in_area.y -= PC_WIN_UL_Y;
		if (point_in_area.in(pc_help_button)) {
			pc_help_button.offset(PC_WIN_UL_X,PC_WIN_UL_Y);
			arrow_button_click(pc_help_button);
			pc_help_button.offset(-PC_WIN_UL_X,-PC_WIN_UL_Y);
			cChoiceDlog("help-party").show();
		}
		for (i = 0; i < 6; i++)
			for (j = 0; j < 5; j++)
				if((pc_area_button_active[i][j] > 0) && (point_in_area.in(pc_buttons[i][j]))) {
					pc_buttons[i][j].offset(PC_WIN_UL_X,PC_WIN_UL_Y);
					arrow_button_click(pc_buttons[i][j]);
					pc_buttons[i][j].offset(-PC_WIN_UL_X,-PC_WIN_UL_Y);
					switch (j) {
						case 0:
							if ((prime_time() == false) && (overall_mode != MODE_SHOPPING)&& (overall_mode != MODE_TALKING))
								add_string_to_buf("Set active: Finish what you're doing first.");
							else if (is_combat())
								add_string_to_buf("Set active: Can't set this in combat.");
							else if ((univ.party[i].main_status != 1) &&
									 ((overall_mode != MODE_SHOPPING) || (store_shop_type != 3)))
								add_string_to_buf("Set active: PC must be here & active.");
							else {
								current_pc = i;
								set_stat_window (i);
								if (overall_mode == MODE_SHOPPING)
									sprintf((char *) str,"Now shopping: %s",univ.party[i].name.c_str());
								else sprintf((char *) str,"Now active: %s",univ.party[i].name.c_str());
								add_string_to_buf((char *)str);
								adjust_spell_menus();
							}
							break;
						case 1:
							sprintf((char *) str,"%s has %d health out of %d.",univ.party[i].name.c_str(),
									univ.party[i].cur_health,univ.party[i].max_health);
							add_string_to_buf((char *)str);
							break;
						case 2:
							sprintf((char *) str,"%s has %d spell pts. out of %d.",univ.party[i].name.c_str(),
									univ.party[i].cur_sp,univ.party[i].max_sp);
							add_string_to_buf((char *)str);
							break;
						case 3: // pc info
							give_pc_info(i);
							break;
						case 4: // trade places
							if (prime_time() == false)
								add_string_to_buf("Trade places: Finish what you're doing first.");
							else if (is_combat())
								add_string_to_buf("Trade places: Can't do this in combat.");
							else {
								switch_pc(i);
							}
							break;
					}
				}
		need_reprint = true;
		put_pc_screen();
		put_item_screen(stat_window,0);
		if (overall_mode == MODE_SHOPPING) {
			set_up_shop_array();
			draw_shop_graphics(0,pc_buttons[0][0]);
		}
	}
	
	
	// Process clicks in item stats area
	if (the_point.in(item_area_rect)) {
		point_in_area = the_point;
		point_in_area.x -= ITEM_WIN_UL_X;
		point_in_area.y -= ITEM_WIN_UL_Y;
		
		for (i = 0; i < 9; i++)
			if ((item_bottom_button_active[i] > 0) && (point_in_area.in(item_screen_button_rects[i]))) {
				item_screen_button_rects[i].offset(ITEM_WIN_UL_X,ITEM_WIN_UL_Y);
				arrow_button_click(item_screen_button_rects[i]);
				item_screen_button_rects[i].offset(-ITEM_WIN_UL_X,-ITEM_WIN_UL_Y);
				switch (i) {
					case 0: case 1: case 2: case 3: case 4: case 5:
						if ((prime_time() == false) && (overall_mode != MODE_TALKING) && (overall_mode != MODE_SHOPPING))
							add_string_to_buf("Set active: Finish what you're doing first.");
						else {
							if (!(is_combat())) {
								if ((univ.party[i].main_status != 1) &&
									((overall_mode != MODE_SHOPPING) || (store_shop_type != 12)))
									add_string_to_buf("Set active: PC must be here & active.");
								else {
									current_pc = i;
									sprintf((char *) str,"Now active: %s",univ.party[i].name.c_str());
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
						give_help(50,0);
						set_stat_window(6);
						break;
					case 8: // help
						cChoiceDlog("help-inventory.xml").show();
						break;
				}
			}
		if (stat_window < 7) {
			for (i = 0; i < 8; i++)
				for (j = 0; j < 6; j++)
					if((item_area_button_active[i][j] > 0) && (point_in_area.in(item_buttons[i][j]))) {
						item_buttons[i][j].offset(ITEM_WIN_UL_X,ITEM_WIN_UL_Y);
						arrow_button_click(item_buttons[i][j]);
						item_buttons[i][j].offset(-ITEM_WIN_UL_X,-ITEM_WIN_UL_Y);
						
						item_hit = item_sbar->getPosition() + i;
						if ((prime_time() == false) && (j < 4)
							&& ((j > 0) || (stat_screen_mode < 2)))
							add_string_to_buf("Item action: Finish what you're doing first.");
						else switch (j) {
							case 0: // equip
								if (overall_mode == MODE_USE_TOWN) {
									add_string_to_buf("Note: Clicking 'U' button by item");
									add_string_to_buf("  uses the item.");
									use_item(stat_window, item_hit);
									overall_mode = MODE_TOWN;
									take_ap(3);
								}
								else if (prime_time() == true) {
									equip_item(stat_window, item_hit);
									take_ap(1);
								}
								break;
							case 1: // use
								use_item(stat_window, item_hit);
								if ((overall_mode != MODE_TOWN_TARGET) && (overall_mode != MODE_SPELL_TARGET))
									did_something = true;
								take_ap(3);
								break;
							case 2: // give
								give_thing(stat_window, item_hit);
								did_something = true;
								take_ap(1);
								break;
							case 3: // drop
								if (stat_window == 6){
									use_spec_item(spec_item_array[item_hit]);
									need_redraw = true;////
								}
								else if (is_out())
									drop_item(stat_window,item_hit,univ.party.p_loc);
								else {
									add_string_to_buf("Drop item: Click where to drop item.");
									store_drop_item = item_hit;
									overall_mode = (is_town()) ? MODE_DROP_TOWN : MODE_DROP_COMBAT;
								}
								break;
							case 4: // info
								if (stat_window == 6)
									put_spec_item_info(spec_item_array[item_hit]);
								else display_pc_item(stat_window, item_hit,univ.party[stat_window].items[item_hit],0);
								break;
							case 5: // sell? That this codes was reached indicates that the item was sellable
								switch (stat_screen_mode) {
									case 2: // identify item
										if (take_gold(shop_identify_cost,false) == false)
											ASB("Identify: You don't have the gold.");
										else {
											play_sound(68);
											ASB("Your item is identified.");
											univ.party[stat_window].items[item_hit].ident = true;
										}
										break;
									case 3: case 4: case 5: // various selling
										play_sound(-39);
										univ.party.gold += store_selling_values[i];
										ASB("You sell your item.");
										take_item(stat_window,item_hit);
										break;
									case 6: // enchant item
										if (take_gold(store_selling_values[i],false) == false)
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
		need_reprint = true;
	}
	
	
	// MARK: Alchemy
	if (the_point.x == 1000) {
		need_reprint = true;
		need_redraw = true;
		if (overall_mode == MODE_TOWN) {
			do_alchemy();
		}
		else add_string_to_buf("Alchemy: Only in town.");
	}
	
	
	
	// MARK: Wait 40 spaces (town only)
	if (the_point.x == 1001) {
		need_reprint = true;
		need_redraw = true;
		
		if (party_sees_a_monst() == true)
			add_string_to_buf("Long wait: Monster in sight.");
		else {
			add_string_to_buf("Long wait...                    ");
			print_buf();
			play_sound(-20);
			draw_rest_screen();
			pause(10);
			for (i = 0; i < 6; i++) {
				store_sp[i] = univ.party[i].cur_health;
				univ.party[i].status[6] = 0;
			}
		}
		i = 0;
		
		while ((i < 80) && (party_sees_a_monst() == false)){
			increase_age();
			do_monsters();
			do_monster_turn();
			j = get_ran(1,1,160 - univ.town.difficulty);
			if (j == 10)
				create_wand_monst();
			for (j = 0; j < 6; j++)
				if (univ.party[j].cur_health < store_sp[j]) {
					i = 200;
					j = 6;
					add_string_to_buf("  Waiting interrupted.");
				}
			if (party_sees_a_monst() == true) {
				i = 200;
				add_string_to_buf("  Monster sighted!");
			}
			else i++;
		}
		put_pc_screen();
	}
	
 	// MARK: If in combat and pc delayed, jump forward a step
 	if (pc_delayed == true) {
 		draw_terrain();
		//pause(2);
		current_pc++;
		combat_next_step();
		set_stat_window(current_pc);
		put_pc_screen();
	}
 	
 	// MARK: At this point, see if any specials have been queued up, and deal with them
 	for (i = 0; i < 20; i++)
		if (special_queue[i].spec >= 0) {
			long long store_time = univ.party.age;
			univ.party.age = special_queue[i].trigger_time;
			s3 = 0;
			run_special(special_queue[i].mode,special_queue[i].type,special_queue[i].spec,
						special_queue[i].where,&s1,&s2,&s3);
			special_queue[i].spec = -1;
			long long change_time = univ.party.age - special_queue[i].trigger_time;
			univ.party.age = store_time + change_time;
			if (s3 > 0)
				draw_terrain();
		}
	
	// MARK: Handle non-PC stuff (like monsters) if the party actually did something
	if (did_something == true) {
		draw_map(true); // TODO: Might be possible to only do this in certain circumstances?
		play_ambient_sound();
		
		if ((overall_mode >= MODE_COMBAT) && (overall_mode < MODE_TALKING)) {
			if (no_pcs_left() == true) {
				end_combat();
				if (which_combat_type == 0) {
					end_town_mode(0,univ.party.p_loc);
					add_string_to_buf("Fled the combat.        ");
					handle_wandering_specials(0,2);
				}
			}
			else {
				
				if (need_redraw == true) {
					draw_terrain();
					if ((combat_active_pc == 6) || (univ.party[combat_active_pc].ap > 0))
						need_redraw = false;
				}
				//pause(2);
				store_cur_pc = current_pc;
				if (combat_next_step() == true)
					need_redraw = true;
				/*					if ((store_cur_pc != current_pc) && (combat_active_pc == 6)) {
				 //			need_reprint = true;
				 need_redraw = true;
				 } */
				
			}
		}
		else {
			increase_age();
			if (!(is_out()) || ((is_out()) && (univ.party.age % 10 == 0))) // no monst move is party outdoors and on horse
				do_monsters();
			if (overall_mode != MODE_OUTDOORS)
				do_monster_turn();
			// Wand monsts
			if ((overall_mode == MODE_OUTDOORS) && (party_toast() == false) && (univ.party.age % 10 == 0)) {
				
				i = get_ran(1,1,70 + PSD[SDF_LESS_WANDER_ENC] * 200);
				if (i == 10)
					create_wand_monst();
				for (i = 0; i < 10; i++)
					if (univ.party.out_c[i].exists == true)
						if (((adjacent(univ.party.p_loc,univ.party.out_c[i].m_loc) == true) ||
							 (univ.party.out_c[i].what_monst.cant_flee >= 10))
							&& (univ.party.in_boat < 0) && (flying() == false)) {
							store_wandering_special = univ.party.out_c[i].what_monst;
							if (handle_wandering_specials(0,0) == true)
								initiate_outdoor_combat(i);
							univ.party.out_c[i].exists = false;
							
							// Get rid of excess keyclicks
							flushingInput = true;
							
							need_reprint = false;
							i = 20;
						}
			}
			if (overall_mode == MODE_TOWN) {
				i = get_ran(1,1,160 - univ.town.difficulty + PSD[SDF_LESS_WANDER_ENC] * 200);
				if (i == 2)
					create_wand_monst();
			}
			
		}
		
		
	}
	
	if (need_redraw == true) {
		draw_terrain();
	}
	
	
	if ((need_reprint == true) || (need_redraw == true)) {
		print_buf();
	}
	
	if (end_scenario) {
		reload_startup();
		overall_mode = MODE_STARTUP;
		draw_startup(0);
		menu_activate();
		univ.party.scen_name = ".exs"; // should be harmless...
		if(cChoiceDlog("congrats-save.xml",{"cancel","save"}).show() == "save"){
			fs::path file = nav_put_party();
			if(!file.empty()) save_party(file);
		}
	}
	else if (party_toast() == true) {
		for (i = 0; i < 6; i++)
			if (univ.party[i].main_status == MAIN_STATUS_FLED) {
				univ.party[i].main_status = MAIN_STATUS_ALIVE;
				if (is_combat()) {
					end_town_mode(0,univ.town.p_loc);
					add_string_to_buf("End combat.               ");
					handle_wandering_specials(0,2);
					
				}
			}
		if (univ.party.is_split()) {
			ASB(univ.party.end_split(0));
			update_explored(univ.town.p_loc);
			center = univ.town.p_loc;
			if (is_combat()) {
				overall_mode = MODE_TOWN;
			}
			else if (is_town()) {
				
			}
			center = univ.town.p_loc;
		}
		menu_activate();
		draw_terrain();
		put_pc_screen();
		put_item_screen(stat_window,0);
		if (party_toast() == true) {
			play_sound(13);
			handle_death();
			if (All_Done == true)
				return true;
		}
	}
	
	
	are_done = All_Done;
	return are_done;
}

bool someone_awake()
{
	short i;
	
	for (i = 0; i < 6; i++)
		if ((univ.party[i].main_status == 1) && (univ.party[i].status[11] <= 0) && (univ.party[i].status[12] <= 0))
			return true;
	return false;
}


void handle_menu_spell(short spell_picked,short spell_type)
{
	location pass_point;
	sf::Event event;
	
	spell_forced = true;
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
	pass_point.x = bottom_buttons[spell_type].left + 5;
	pass_point.y = bottom_buttons[spell_type].top + 5;
	event.mouseButton.x = pass_point.x + ul.x;
	event.mouseButton.y = pass_point.y + ul.y;
	handle_action(event);
}

void initiate_outdoor_combat(short i)
{
	short m,n;
	location to_place;
	
	draw_terrain();
	
	// Is combat too easy?
	if ((party_total_level() > ((out_enc_lev_tot(i) * 5) / 3) ) && (out_enc_lev_tot(i) < 200)
		&& (univ.party.out_c[i].what_monst.cant_flee % 10 != 1)) {
		add_string_to_buf("Combat: Monsters fled!           ");
		univ.party.out_c[i].exists = false;
		return;
	}
	
//	Delay((long) 100,&dummy);
	
	start_outdoor_combat(univ.party.out_c[i], univ.out[univ.party.p_loc.x][univ.party.p_loc.y],count_walls(univ.party.p_loc));
	
	univ.party.out_c[i].exists = false;
	
	for (m = 0; m < 6; m++)
		if (univ.party[m].main_status == 1)
			to_place = pc_pos[m];
	for (m = 0; m < 6; m++)
		for (n = 0; n < 24; n++)
			if ((univ.party[m].main_status != 1) && (univ.party[m].items[n].variety != 0)) {
				place_item(univ.party[m].items[n],to_place,true);
				univ.party[m].items[n].variety = ITEM_TYPE_NO_ITEM;
			}
	
	overall_mode = MODE_COMBAT;
	center = pc_pos[current_pc];
	draw_terrain();
}

bool handle_keystroke(sf::Event& event){
	bool are_done = false;
	location pass_point; // TODO: This isn't needed
	short i,j;
	std::ostringstream sout;
	using kb = sf::Keyboard;
	using Key = sf::Keyboard::Key;
	
	Key keypad[10] = {kb::Numpad0,kb::Numpad1,kb::Numpad2,kb::Numpad3,kb::Numpad4,kb::Numpad5,kb::Numpad6,kb::Numpad7,kb::Numpad8,kb::Numpad9};
	// TODO: The duplication of location here shouldn't be necessary...
	location terrain_click[10] = {location{150,185},location{120,215},location{150,215},location{180,215},
		location{120,185},location{150,185},location{180,185},
		location{120,155},location{150,155},location{180,135}};
	Key talk_chars[9] = {kb::L,kb::N,kb::J,kb::B,kb::S,kb::R,kb::D,kb::G,kb::A};
	Key shop_chars[8] = {kb::A,kb::B,kb::C,kb::D,kb::E,kb::F,kb::G,kb::H};
	
	if(map_visible && isFrontWindow(mini_map) && event.key.code == kb::Escape
	   && (overall_mode != MODE_TALKING) && (overall_mode != MODE_SHOPPING)) {
		mini_map.setVisible(false);
		map_visible = false;
		mainPtr.setActive();
		return false;
	}
	
	if(overall_mode == MODE_STARTUP)
		return false;
	
	// Only hide the cursor if it's in the window.
	// This is because we want to show it as soon as the mouse moves, but we don't receive mouse move events if it's outside the window.
	if(sf::IntRect(mainPtr.getPosition(), sf::Vector2i(mainPtr.getSize())).contains(sf::Mouse::getPosition()))
		mainPtr.setMouseCursorVisible(false);
	
	// DEBUG
//	sprintf((char *) debug, "%d    ",(short) chr2);
//	add_string_to_buf((char *) debug);
//	print_buf();
	Key chr2 = event.key.code;
	
	if(chr2 == kb::LShift || chr2 == kb::LAlt || chr2 == kb::LControl || chr2 == kb::LSystem) return false;
	if(chr2 == kb::RShift || chr2 == kb::RAlt || chr2 == kb::RControl || chr2 == kb::RSystem) return false;
	
	if(chr2 == kb::Up && !kb::isKeyPressed(kb::Down)) {
		if(kb::isKeyPressed(kb::Left)) chr2 = kb::Numpad7;
		else if(kb::isKeyPressed(kb::Right)) chr2 = kb::Numpad9;
		else chr2 = kb::Numpad8;
	} else if(chr2 == kb::Down && !kb::isKeyPressed(kb::Up)) {
		if(kb::isKeyPressed(kb::Left)) chr2 = kb::Numpad1;
		else if(kb::isKeyPressed(kb::Right)) chr2 = kb::Numpad3;
		else chr2 = kb::Numpad2;
	} else if(chr2 == kb::Left && !kb::isKeyPressed(kb::Right)) {
		if(kb::isKeyPressed(kb::Up)) chr2 = kb::Numpad7;
		else if(kb::isKeyPressed(kb::Down)) chr2 = kb::Numpad1;
		else chr2 = kb::Numpad4;
	} else if(chr2 == kb::Right && !kb::isKeyPressed(kb::Left)) {
		if(kb::isKeyPressed(kb::Up)) chr2 = kb::Numpad9;
		else if(kb::isKeyPressed(kb::Down)) chr2 = kb::Numpad3;
		else chr2 = kb::Numpad6;
	}
	
	sf::Event pass_event = {sf::Event::MouseButtonPressed};
	if (overall_mode == MODE_TALKING) {
		if (chr2 == kb::Escape)
			chr2 = kb::D;
		if (chr2 == kb::Space)
			chr2 = kb::G;
		for (i = 0; i < 9; i++)
			if (chr2 == talk_chars[i]) {
				pass_point.x = preset_words[i].word_rect.left + 9 + ul.x;
				pass_point.y = preset_words[i].word_rect.top + 9 + ul.y;
				pass_event.mouseButton.x = pass_point.x;
				pass_event.mouseButton.y = pass_point.y;
				are_done = handle_action(pass_event);
			}
	}
	else if (overall_mode == MODE_SHOPPING) { // shopping keystrokes
		if (chr2 == kb::Escape) {
			pass_point.x = 222 + ul.x;
			pass_point.y = 398 + ul.y;
			pass_event.mouseButton.x = pass_point.x;
			pass_event.mouseButton.y = pass_point.y;
			are_done = handle_action(pass_event);
		}
		for (i = 0; i < 8; i++)
			if (chr2 == shop_chars[i]) {
				pass_point.x = shopping_rects[i][1].left + 9 + ul.x;
				pass_point.y = shopping_rects[i][1].top + 9 + ul.y;
				pass_event.mouseButton.x = pass_point.x;
				pass_event.mouseButton.y = pass_point.y;
				are_done = handle_action(pass_event);
			}
	} else {
		for (i = 0; i < 10; i++)
			if (chr2 == keypad[i]) {
				if (i == 0) {
					chr2 = kb::Z;
				}
				else {
					pass_point.x = terrain_click[i].x + ul.x;
					pass_point.y = terrain_click[i].y + ul.y;
					pass_event.mouseButton.x = pass_point.x;
					pass_event.mouseButton.y = pass_point.y;
					are_done = handle_action(pass_event);
					return are_done;
				}
			}
	}
	
	char chr = keyToChar(chr2, event.key.shift);
	
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
			if (overall_mode == MODE_SHOPPING) {
				univ.party.help_received[26] = 0;
				give_help(226,27);
				break;
			}
			if (overall_mode == MODE_TALKING) {
				univ.party.help_received[5] = 0;
				give_help(205,6);
				break;
			}
			if (is_out()) cChoiceDlog("help-outdoor.xml").show();
			if (is_town()) cChoiceDlog("help-town.xml").show();
			if (is_combat()) cChoiceDlog("help-combat.xml").show();
			break;
			
		case '1': case '2': case '3': case '4': case '5': case '6':
			pass_point.x = pc_buttons[((short) chr) - 49][0].left + 1 + PC_WIN_UL_X + ul.x;
			pass_point.y = pc_buttons[((short) chr) - 49][0].top + PC_WIN_UL_Y + ul.y;
			pass_event.mouseButton.x = pass_point.x;
			pass_event.mouseButton.y = pass_point.y;
			are_done = handle_action(pass_event);
			break;
			
		case '9':
			pass_point.x = item_screen_button_rects[6].left + ITEM_WIN_UL_X + ul.x;
			pass_point.y = item_screen_button_rects[6].top + ITEM_WIN_UL_Y + ul.y;
			pass_event.mouseButton.x = pass_point.x;
			pass_event.mouseButton.y = pass_point.y;
			are_done = handle_action(pass_event);
			break;
			
		case ' ':
			if (overall_mode == MODE_FANCY_TARGET) { // cast multi-target spell, set # targets to 0 so that
				// space clicked doesn't matter
				num_targets_left = 0;
				pass_point = terrain_click[5];
				pass_event.mouseButton.x = pass_point.x + ul.x;
				pass_event.mouseButton.y = pass_point.y + ul.y;
				are_done = handle_action(pass_event);
			}
			if (overall_mode == MODE_SPELL_TARGET)
				spell_cast_hit_return();
			break;
			
			
		case 'D':
			if (in_scen_debug) {
				in_scen_debug = false;
				ASB("Debug mode OFF.");
			} else {
				in_scen_debug = true;
				ASB("Debug mode ON.");
			}
			print_buf();
			break;
		case 'z':
			if (((overall_mode >= MODE_COMBAT) && (overall_mode < MODE_TALKING)) || (overall_mode == MODE_LOOK_COMBAT)) {
				set_stat_window(current_pc);
				put_item_screen(stat_window,0);
			} else {
				set_stat_window(0);
				put_item_screen(stat_window,0);
			}
			break;
			
		case '=':
			if(!in_scen_debug) break;
			univ.party.gold += 100;
			univ.party.food += 100;
			for (i = 0; i < 6; i++) {
				univ.party[i].main_status = MAIN_STATUS_ALIVE;
				univ.party[i].cur_health = univ.party[i].max_health;
				univ.party[i].cur_sp = 100;
			}
			award_party_xp(25);
			for (i = 0; i < 6; i++)
				for (j = 0; j < 62; j++) {
					univ.party[i].priest_spells[j] = 1;
					univ.party[i].mage_spells[j] = 1;
				}
			refresh_store_items();
			add_string_to_buf("Debug: Add stuff and heal.");
			print_buf();
			put_pc_screen();
			break;
			
		case 'B':
			if(!in_scen_debug) break;
			for(i=0;i<6;i++)
                if(univ.party[i].main_status >= MAIN_STATUS_SPLIT)
                    univ.party[i].main_status -= MAIN_STATUS_SPLIT;
			if(overall_mode == MODE_OUTDOORS){
				add_string_to_buf("Debug - Leave Town: You're not in town!");
				print_buf();
				break;
			}
			overall_mode = MODE_OUTDOORS;
			position_party(univ.party.outdoor_corner.x,univ.party.outdoor_corner.y,univ.party.p_loc.x,univ.party.p_loc.y);
			clear_map();
			add_string_to_buf("Debug: Reunite party and leave town.");
			print_buf();
			redraw_screen(REFRESH_ALL);
			break;
			
		case 'C':
			if(!in_scen_debug) break;
			for (i = 0; i < 6; i++) {
				univ.party[i].status[STATUS_POISON] = 0;
				if(univ.party[i].status[STATUS_BLESS_CURSE] < 0)
					univ.party[i].status[STATUS_BLESS_CURSE] = 0;
				if (univ.party[i].status[STATUS_HASTE_SLOW] < 0)
					univ.party[i].status[STATUS_HASTE_SLOW] = 0;
				univ.party[i].status[STATUS_WEBS] = 0;
				univ.party[i].status[STATUS_DISEASE] = 0;
				univ.party[i].status[STATUS_DUMB] = 0;
				univ.party[i].status[STATUS_ASLEEP] = 0;
				univ.party[i].status[STATUS_PARALYZED] = 0;
				univ.party[i].status[STATUS_ACID] = 0;
			}
			add_string_to_buf("Debug: You get cleaned up!");
			print_buf();
			put_pc_screen();
			break;
			
		case 'E':
			if(!in_scen_debug) break;
			PSD[SDF_PARTY_STEALTHY] += 10;
			PSD[SDF_PARTY_DETECT_LIFE] += 10;
			PSD[SDF_PARTY_FIREWALK] += 10;
			add_string_to_buf("Debug: Stealth, Detect Life, Firewalk!");
			print_buf();
			put_pc_screen();
			break;
			
		case 'F':
			if(!in_scen_debug) break;
			if(overall_mode != MODE_OUTDOORS){
				add_string_to_buf("Debug: Can only fly outdoors.");
			}else{
				PSD[SDF_PARTY_FLIGHT] += 10;
				add_string_to_buf("Debug: You start flying!");
			}
			print_buf();
			put_pc_screen();
			break;
			
		case 'G':
			if(!in_scen_debug) break;
			if(ghost_mode){
				ghost_mode = false;
				ASB("Debug: Ghost mode OFF.");
			}else{
				ghost_mode = true;
				ASB("Debug:Ghost mode ON.");
			}
			print_buf();
			break;
			
		case 'H':
			if(!in_scen_debug) break;
			univ.party.gold += 100;
			univ.party.food += 100;
			for (i = 0; i < 6; i++) {
				if ((univ.party[i].main_status > MAIN_STATUS_ALIVE) && (univ.party[i].main_status < MAIN_STATUS_FLED))
					univ.party[i].main_status = MAIN_STATUS_ALIVE;
			}
			heal_party(250);
			restore_sp_party(100);
			add_string_to_buf("Debug: Heal party.");
			print_buf();
			put_pc_screen();
			break;
			
		case 'K':
			if (!in_scen_debug) break;
			for (i = 0; i < univ.town->max_monst(); i++) {
				if ((is_combat()) && (univ.town.monst[i].active > 0) && (univ.town.monst[i].attitude % 2 == 1))
					univ.town.monst[i].active = 0;
				
				if ((univ.town.monst[i].active > 0) && (univ.town.monst[i].attitude % 2 == 1)
					&& (dist(univ.town.monst[i].cur_loc,univ.town.p_loc) <= 10) )
					damage_monst(i, 7,1000,0, DAMAGE_UNBLOCKABLE,0);
			}
			// kill_monst(&univ.town.monst[i],6);
			draw_terrain();
			add_string_to_buf("Debug: Kill things.            ");
			print_buf();
			break;
			
		case 'N':
			if(!in_scen_debug) break;
			end_scenario = true;
			in_scen_debug = false;
			break;
			
		case 'O':
			if(!in_scen_debug) break;
			if (is_town()) {
				sout << "Debug:  You're at x " << (short) univ.town.p_loc.x << ", y " << (short) univ.town.p_loc.y
				<< " in town " << univ.town.num << '.';
			} else if (is_out()) {
				short x = univ.party.p_loc.x;
				short y = univ.party.p_loc.y;
				x += 48 * univ.party.outdoor_corner.x;
				y += 48 * univ.party.outdoor_corner.y;
				sout << "Debug:  You're outside at x " << x << ", y " << y << ',';
			}
			add_string_to_buf(sout.str());
			print_buf();
			break;
			
		case 'I': // TODO: Seems useless?
//			if(!in_scen_debug) break;
//			sout << "Debug: The party's age is " << univ.party.age;
//			add_string_to_buf(sout.str());
//			add_string_to_buf("Debug: Reset map."); // Surely this won't work?
//			print_buf();
			break;
			
		case 'Q':
			if(!in_scen_debug) break;
			if (overall_mode == MODE_OUTDOORS) {
				for (i = 0; i < 96; i++)
					for (j = 0; j < 96; j++)
						make_explored(i,j);
			} else {
				for (i = 0; i < 64; i++)
					for (j = 0; j < 64; j++)
						make_explored(i,j);
			}
			clear_map();
			add_string_to_buf("Debug:  Magic Map.");
			print_buf();
			break;
			
		case 'R':
			if(!in_scen_debug) break;
			if (univ.party.in_boat >= 0) {
				add_string_to_buf("  Not while in boat. ");
				break;
			}
			if (univ.party.in_horse >= 0) {
				add_string_to_buf("  Not while on horse. ");
				break;
			}
			force_town_enter(scenario.which_town_start,scenario.where_start);
			start_town_mode(scenario.which_town_start,9);
			position_party(scenario.out_sec_start.x,scenario.out_sec_start.y,
						   scenario.out_start.x,scenario.out_start.y);
			center = univ.town.p_loc = scenario.where_start;
			redraw_screen(REFRESH_ALL);
			add_string_to_buf("Debug:  You return to the start.");
			print_buf();
			break;
			
		case 'S': // TODO: Create a dedicated dialog for this.
			if (!in_scen_debug) break;
			cStrDlog("Enter Stuff Done Flag Part A (between 1 and 299)","","Which SDFa ?",130,PIC_DLOG).show();
            i = atoi(get_text_response(873,0).c_str());
            if(i > 0 && i < 300){
				cStrDlog("Enter Stuff Done Flag Part B (between 0 and 49)","","Which SDFb ?",130,PIC_DLOG).show();
				j = atoi(get_text_response(873,0).c_str());
				if(j >= 0 && j < 50){
					cStrDlog("Enter Stuff Done Flag Value (up to 255)","","Which value ?",130,PIC_DLOG).show();
					int x = atoi(get_text_response(873,0).c_str());
					if(x < 256 && x >= 0)
						PSD[i][j] = x;
					else if(x == -1){
						sout << "SDF(" << i << ',' << j << ") = " << PSD[i][j];
						add_string_to_buf(sout.str());
					}
				}
			}
			break;
			
		case 'T':
			// TODO: Create a dedicated dialog for this?
			if(!in_scen_debug) break;
			short find_direction_from;
			sout << "Enter Town Number (between 0 and " << scenario.num_towns - 1 << ')';
           	cStrDlog(sout.str(),"","Which Town ?",130,PIC_DLOG).show();
            i = atoi(get_text_response(873,0).c_str());
            if(i >= 0 && i < scenario.num_towns ){
            	if (univ.party.direction == 0) find_direction_from = 2;
				else if (univ.party.direction == 4) find_direction_from = 0;
				else if (univ.party.direction < 4) find_direction_from = 3;
				else find_direction_from = 1;
				start_town_mode(i, find_direction_from);
			}
			break;
			
		case 'W':
			refresh_store_items();
			add_string_to_buf("Debug: Refreshed jobs/shops.            ");
			print_buf();
			break;
			
//		case '`':
//			break;
//		case '[':
//			break;
//		case '+':
//			break;
		case '<':
			//break;
			if (!in_scen_debug) break;
			ASB("Debug: Increase age.");
			ASB("  It is now 1 day later.");
			print_buf();
			univ.party.age += 3700;
			put_pc_screen();
			break;
		case '>':
			ASB("DEBUG: Towns have short memory.");
			ASB("Your deeds have been forgotten.");
			print_buf();
			for (i = 0; i < 4; i++)
				univ.party.creature_save[i].which_town = 200;
			break;
		case '/':
			if(!in_scen_debug) break;
			ASB("Debug hot keys");
			ASB("  B  Leave town");
			ASB("  C  Get cleaned up");
			ASB("  D  Toggle Debug mode");
			ASB("  E  Stealth, Detect Life, Firewalk");
			ASB("  F  Flight");
			ASB("  G  Ghost");
			ASB("  H  Heal");
			ASB("  K  Kill things");
			ASB("  N  End Scenario");
			ASB("  O  Location");
			ASB("  Q  Magic map");
			ASB("  R  Return to Start");
			ASB("  S  Set a SDF");
			ASB("  T  Enter Town");
			ASB("  W  Refresh jobs/shops");
			ASB("  =  Heal, increase magic skills");
			ASB("  <  Make one day pass");
			ASB("  >  Towns forgive you");
			ASB("  /  Bring up this list");
			print_buf();
			break;
		case 'a':
			if (overall_mode < MODE_TALK_TOWN) {
				pass_point.x = (overall_mode == MODE_OUTDOORS) ? 170 : 221;
				pass_point.y = 405;
				pass_event.mouseButton.x = pass_point.x + ul.x;
				pass_event.mouseButton.y = pass_point.y + ul.y;
				are_done = handle_action(pass_event);
			}
			break;
			
		case 'b': case 'u': case 'L':
			if (overall_mode == MODE_TOWN) {
				pass_point.x = (chr == 'u') ? 220 : 205;
				pass_point.y = (chr == 'L') ? 405 : 388;
				pass_event.mouseButton.x = pass_point.x + ul.x;
				pass_event.mouseButton.y = pass_point.y + ul.y;
				are_done = handle_action(pass_event);
			}
			break;
			
		case 's': case 'x': case 'e':
			if ((overall_mode == MODE_COMBAT) ||
				((overall_mode == MODE_FIRING)  && (chr == 's')) ||
				((overall_mode == MODE_THROWING)  && (chr == 's')) ) {
				pass_point.x = (chr == 's') ? 205 : 240;
				pass_point.y = (chr == 'e') ? 390 : 406;
				pass_event.mouseButton.x = pass_point.x + ul.x;
				pass_event.mouseButton.y = pass_point.y + ul.y;
				are_done = handle_action(pass_event);
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
					case 'M': spell_forced = true; j = 0; break;
					case 'm': j = 0; break;
					case 'P': spell_forced = true; j = 1; break;
					case 'p': j = 1; break;
					case 'l': j = 2; break;
					case 'r': if (overall_mode != MODE_OUTDOORS) return false;
						j = 3;
						break;
					case 't': if (overall_mode == MODE_TOWN)
						j = 3;
					else return false;
						break;
					case 'A':if (overall_mode == MODE_TOWN) {
						// TODO: Uh, what about ul.y?
						pass_point.x = 1000 + ul.x;
						pass_event.mouseButton.x = pass_point.x;
						pass_event.mouseButton.y = pass_point.y;
						are_done = handle_action(pass_event);
					}
					else {
						add_string_to_buf("Alchemy: In town only.");
						print_buf();
						return false;
					}
						break;
					case 'w':if (overall_mode == MODE_COMBAT)
						j = 5;
					else if (overall_mode == MODE_TOWN) {
						// TODO: Uh, what about ul.y?
						pass_point.x = 1001 + ul.x;
						pass_event.mouseButton.x = pass_point.x;
						pass_event.mouseButton.y = pass_point.y;
						are_done = handle_action(pass_event);
					}
					else {
						add_string_to_buf("Wait: In town only.");
						print_buf();
						return false;
					}
						break;
					case 'd': if (overall_mode != MODE_COMBAT) return false;
						j = 3;
						break;
					case 'g': if (overall_mode == MODE_OUTDOORS) return false;
						j = 4;
						break;
					case 'f': if (overall_mode != MODE_TOWN) return false;
						j = 6;
						break;
				}
			}
			if (j < 50) {
				pass_point.x = bottom_buttons[j].left + 5;
				pass_point.y = bottom_buttons[j].top + 5;
				pass_event.mouseButton.x = pass_point.x + ul.x;
				pass_event.mouseButton.y = pass_point.y + ul.y;
				are_done = handle_action(pass_event);
			}
			break;
	}
	spell_forced = false;
	return are_done;
}

void do_load()
{
	fs::path file_to_load = nav_get_party();
	if(!file_to_load.empty()) load_party(file_to_load);
	if(overall_mode != MODE_STARTUP)
		post_load();
	menu_activate();
}

void post_load()
{
	current_switch = 6;
	
	reset_item_max();
	
	if (overall_mode == MODE_OUTDOORS)
		update_explored(univ.party.p_loc);
//	if (overall_mode == MODE_TOWN) {
//		make_town_trim(0);
//		}
//	make_out_trim();
	
	// TODO: Presumably need to call draw() as well, but maybe not here
	text_sbar->show();
	item_sbar->show();
	shop_sbar->hide();
	set_stat_window(0);
	put_pc_screen();
	draw_terrain();
	draw_buttons(0);
	draw_text_bar(1);
	
	print_buf();
	
	clear_map();
	adjust_spell_menus();
	adjust_monst_menu();
}

void do_save(short mode)
//mode; // 0 - normal  1 - save as
{
	if (overall_mode > MODE_TOWN) {
		add_string_to_buf("Save: Only while outdoors, or in         ");
		add_string_to_buf("  town and not looking/casting.          ");
		print_buf();
		return;
	}
	fs::path file = univ.file;
	if(mode == 1) file = nav_put_party();
	if(!file.empty()) {
		univ.file = file;
		save_party(univ.file);
	}
	
	
	pause(6);
	redraw_screen(REFRESH_TEXT);
}

void increase_age()////
{
	short i,j,item,how_many_short = 0,r1,store_day;
	bool update_stat = false;
	
	
	// Increase age, adjust light level & stealth
	store_day = calc_day();
	if (is_out()) {
		if (univ.party.in_horse < 0)
			univ.party.age -= univ.party.age % 10;
		else univ.party.age -= univ.party.age % 5;
		univ.party.age += 5;
		if (univ.party.in_horse < 0)
			univ.party.age += 5;
		
	}
	else univ.party.age++;
	if (calc_day() != store_day) { // Day changed, so check for interesting stuff.
		update_stat = true;
	}
	
	univ.party.light_level = move_to_zero(univ.party.light_level);
	
//	if (PSD[128][9] == 1)
//		clear_map();
	
	// decrease monster present counter
	PSD[SDF_HOSTILES_PRESENT] = move_to_zero(PSD[SDF_HOSTILES_PRESENT]);
	
	// Party spell effects
	if (PSD[SDF_PARTY_STEALTHY] == 1) {reset_text_bar();
		add_string_to_buf("Your footsteps grow louder.      "); }
	PSD[SDF_PARTY_STEALTHY] = move_to_zero(PSD[SDF_PARTY_STEALTHY]);
	if (PSD[SDF_PARTY_DETECT_LIFE] == 1) {reset_text_bar();
		add_string_to_buf("You stop detecting monsters.      ");}
	PSD[SDF_PARTY_DETECT_LIFE] = move_to_zero(PSD[SDF_PARTY_DETECT_LIFE]);
	if (PSD[SDF_PARTY_FIREWALK] == 1) {reset_text_bar();
		add_string_to_buf("Your feet stop glowing.      ");}
	PSD[SDF_PARTY_FIREWALK] = move_to_zero(PSD[SDF_PARTY_FIREWALK]);
	
	if (PSD[SDF_PARTY_FLIGHT] == 2)
		add_string_to_buf("You are starting to descend.");
	if (PSD[SDF_PARTY_FLIGHT] == 1) {
		if (scenario.ter_types[univ.out[univ.party.p_loc.x][univ.party.p_loc.y]].blockage > 2) {
			add_string_to_buf("  You plummet to your deaths.                  ");
			slay_party(MAIN_STATUS_DEAD);
			print_buf();
			pause(150);
		}
		else add_string_to_buf("  You land safely.                  ");
		reset_text_bar();
	}
	
	PSD[SDF_PARTY_FLIGHT] = move_to_zero(PSD[SDF_PARTY_FLIGHT]);
	
	if ((overall_mode > MODE_OUTDOORS) && (univ.town->lighting_type == 2))
		univ.party.light_level = max (0,univ.party.light_level - 9);
	if (univ.town->lighting_type == 3) {
		if (univ.party.light_level > 0)
			ASB("Your light is drained.");
		univ.party.light_level = 0;
	}
	
	
	// Specials countdowns
	if ((univ.party.age % 500 == 0
		 ) && (get_ran(1,0,5) == 3) && (party_has_abil(52) == true)) {
		update_stat = true;
		// TODO: This seems to be the "radioactivity" handler, and the string appears to not exist.
		cStrDlog display_enc_string("Missing String: Radioactivity", "", "", 8, PIC_DLOG);
		display_enc_string.setSound(3);
		display_enc_string.show();
		for (i = 0; i < 6; i++)
			disease_pc(i,2);
	}
	
	
	// Plants and magic shops
	if (univ.party.age % 4000 == 0) {
		//SysBeep(2);
		//ASB("DEBUG: Stuff replaced.");
		refresh_store_items();
		//for (i = 0; i < 10; i++)
		//	PSD[302][i] = 0;
		//for (i = 0; i < 10; i++)
		//	PSD[301][i] = 0;
		//for (i = 0; i < 10; i++)
		////	PSD[300][i] = 0;
		}
		
	// Protection, etc.
	for (i = 0; i < 6; i++) { // Process some status things, and check if stats updated
		
		if ((univ.party[i].status[4] == 1) || (univ.party[i].status[5] == 1) || (univ.party[i].status[8] == 1)
			|| (univ.party[i].status[11] == 1)|| (univ.party[i].status[12] == 1))
			update_stat = true;
		univ.party[i].status[4] = move_to_zero(univ.party[i].status[4]);
		univ.party[i].status[5] = move_to_zero(univ.party[i].status[5]);
		univ.party[i].status[8] = move_to_zero(univ.party[i].status[8]);
		univ.party[i].status[10] = move_to_zero(univ.party[i].status[10]);
		univ.party[i].status[11] = move_to_zero(univ.party[i].status[11]);
		univ.party[i].status[12] = move_to_zero(univ.party[i].status[12]);
		if ((univ.party.age % 40 == 0) && (univ.party[i].status[0] > 0)) {
			update_stat = true;
			univ.party[i].status[0] = move_to_zero(univ.party[i].status[0]);
		}
		
	}
	
	// Food
	if ((univ.party.age % 1000 == 0) && (overall_mode < MODE_COMBAT)) {
		for (i = 0; i < 6; i++)
			if (univ.party[i].main_status == 1)
				how_many_short++;
		how_many_short = take_food (how_many_short,false);
		if (how_many_short > 0) {
			add_string_to_buf("Starving! ");
			play_sound(66);
			r1 = get_ran(3,1,6);
			hit_party(r1,DAMAGE_UNBLOCKABLE);
			update_stat = true;
			if (overall_mode < MODE_COMBAT)
				boom_space(univ.party.p_loc,overall_mode,0,r1,0);
		}
		else {
			play_sound(6);
			add_string_to_buf("You eat.  ");
		}
		update_stat = true;
	}
	
	// Poison, acid, disease damage
	for (i = 0; i < 6; i++) // Poison
		if (univ.party[i].status[2] > 0) {
			i = 6;
			if (((overall_mode == MODE_OUTDOORS) && (univ.party.age % 50 == 0)) || ((overall_mode == MODE_TOWN) && (univ.party.age % 20 == 0))) {
				update_stat = true;
				do_poison();
			}
		}
	for (i = 0; i < 6; i++) // Disease
		if (univ.party[i].status[7] > 0) {
			i = 6;
			if (((overall_mode == MODE_OUTDOORS) && (univ.party.age % 100 == 0)) || ((overall_mode == MODE_TOWN) && (univ.party.age % 25 == 0))) {
				update_stat = true;
				handle_disease();
			}
		}
	for (i = 0; i < 6; i++) // Acid
		if (univ.party[i].status[13] > 0) {
			i = 6;
			update_stat = true;
			handle_acid();
		}
	
	// Healing and restoration of spell pts.
	if (is_out()) {
		if (univ.party.age % 100 == 0) {
			for (i = 0; i < 6; i++)
				if ((univ.party[i].main_status == 1) && (univ.party[i].cur_health < univ.party[i].max_health))
					update_stat = true;
			heal_party(2);
		}
	}
	else {
		if (univ.party.age % 50 == 0) {
			for (i = 0; i < 6; i++)
				if ((univ.party[i].main_status == 1) && (univ.party[i].cur_health < univ.party[i].max_health))
					update_stat = true;
			heal_party(1);
		}
	}
	if (is_out()) {
		if (univ.party.age % 80 == 0) {
			for (i = 0; i < 6; i++)
				if ((univ.party[i].main_status == 1) && (univ.party[i].cur_sp < univ.party[i].max_sp))
					update_stat = true;
			restore_sp_party(2);
		}
	}
	else {
		if (univ.party.age % 40 == 0) {
			for (i = 0; i < 6; i++)
				if ((univ.party[i].main_status == 1) && (univ.party[i].cur_sp < univ.party[i].max_sp))
					update_stat = true;
			restore_sp_party(1);
		}
	}
	
	// Recuperation and chronic disease disads
	for (i = 0; i < 6; i++)
		if (univ.party[i].main_status == 1) {
			if ((univ.party[i].traits[9] > 0) && (get_ran(1,0,10) == 1) && (univ.party[i].cur_health < univ.party[i].max_health)) {
				heal_pc(i,2);
				update_stat = true;
			}
			if ((univ.party[i].traits[13] > 0) && (get_ran(1,0,110) == 1)) {
				disease_pc(i,4);
				update_stat = true;
			}
			
		}
	
	
	// Blessing, slowed,etc.
	if (univ.party.age % 4 == 0)
		for (i = 0; i < 6; i++) {
			if ((univ.party[i].status[1] != 0) || (univ.party[i].status[3] != 0))
				update_stat = true;
			univ.party[i].status[1] = move_to_zero(univ.party[i].status[1]);
			univ.party[i].status[3] = move_to_zero(univ.party[i].status[3]);
			if (((item = pc_has_abil_equip(i,50)) < 24)
				&& (univ.party[i].cur_health < univ.party[i].max_health)
				&& ((overall_mode > MODE_OUTDOORS) || (get_ran(1,0,10) == 5))){
				j = get_ran(1,0,univ.party[i].items[item].ability_strength / 3);
				if (univ.party[i].items[item].ability_strength / 3 == 0)
					j = get_ran(1,0,1);
				if (is_out()) j = j * 4;
				heal_pc(i,j);
				update_stat = true;
			}
		}
	
	dump_gold(1);
	
	special_increase_age();
	//if (debug_on == false)
	push_things();
	
	if (is_town()) {
		process_fields();
	}
	
	
	
	// Cancel switching PC order
	current_switch = 6;
	
	// If a change, draw stat screen
	if (update_stat == true)
		put_pc_screen();
	adjust_spell_menus();
}

void handle_cave_lore()////
{
	char str[60];
	short i,pic;
	ter_num_t ter;
	
	if (!is_out())
		return;
	
	ter = univ.out[univ.party.p_loc.x][univ.party.p_loc.y];
	pic = scenario.ter_types[ter].picture;
	for (i = 0; i < 6; i++)
		if ((univ.party[i].main_status == 1) && (univ.party[i].traits[4] > 0) && (get_ran(1,0,12) == 5)
			&& (((pic >= 0) && (pic <= 1)) || ((pic >= 70) && (pic <= 76))) ) {
			sprintf((char *)str,"%s hunts.",univ.party[i].name.c_str());
			univ.party.food += get_ran(2,1,6);
			add_string_to_buf((char *)str);
			put_pc_screen();
		}
	for (i = 0; i < 6; i++)
		if (
			(univ.party[i].main_status == 1) && (univ.party[i].traits[5] > 0) && (get_ran(1,0,12) == 5)
			&& (((pic >= 2) && (pic <= 4)) || ((pic >= 79) && (pic <= 84)))) {
			sprintf((char *)str,"%s hunts.",univ.party[i].name.c_str());
			univ.party.food += get_ran(2,1,6);
			add_string_to_buf((char *)str);
			put_pc_screen();
		}
	
	
}

void switch_pc(short which)
{
	cPlayer store_pc;
	
	if (current_switch < 6) {
		add_string_to_buf("Switch: OK.");
		if (current_switch != which) {
			store_pc = univ.party[which];
			univ.party[which] = univ.party[current_switch];
			univ.party[current_switch] = store_pc;
			if (current_pc == current_switch)
				current_pc = which;
			else if (current_pc == which)
				current_pc = current_switch;
			set_stat_window(current_pc);
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
	std::string choice;
	
	choice = cChoiceDlog("delete-pc-confirm.xml",{"yes","no"}).show();
	if (choice == "no") {
		add_string_to_buf("Delete PC: Cancelled.           ");
		return;
	}
	add_string_to_buf("Delete PC: OK.                  ");
	kill_pc(which,MAIN_STATUS_ABSENT);
	for(short i = which; i < 5; i++)
		univ.party[i] = univ.party[i + 1];
	univ.party[5].main_status = MAIN_STATUS_ABSENT;
	set_stat_window(0);
	put_pc_screen();
}

void handle_death()
{
	std::string choice;
	
	overall_mode = MODE_STARTUP;
	
	while(true) {
		// Use death (or leave Exile) dialog
		choice = cChoiceDlog("party-death.xml",{"load","new","quit"}).show();
		
		if(choice == "quit") {
			All_Done = true;
			return;
		}
		else if(choice == "load") {
			fs::path file_to_load = nav_get_party();
			if(!file_to_load.empty()) load_party(file_to_load);
			if (party_toast() == false) {
				if(overall_mode != MODE_STARTUP)
					post_load();
            	return;
			}
		}
		else if(choice == "new") {
			start_new_game();
			return;
		}
	}
	
}

void start_new_game()
{
	short i;
	std::string choice;
	using kb = sf::Keyboard;
	
	choice = cChoiceDlog("new-party.xml",{"okay","cancel"}).show();
	if(choice == "cancel")
		return;
	
	//which = choice - 1;
	
//	display_intro();
	if(kb::isKeyPressed(kb::LSystem) || kb::isKeyPressed(kb::RSystem)) init_party(2); // if command key held down, create debug party
	else init_party(0);
	
	//while (creation_done == false) {
	edit_party(1,0);
	/*	if ((i > 0) || (in_startup_mode == false))
			creation_done = true;
		if ((i == 0) && (in_startup_mode == false))
			return;
	} */

	// if no PCs left, forget it
	for (i = 0 ; i < 6; i++)
		if (univ.party[i].main_status == 1)
			i = 100;
	if (i == 6)
		return;
	
	
	// everyone gets a weapon
	for (i = 0; i < 6; i++)
		if (univ.party[i].main_status == 1) {
			univ.party[i].items[0] = start_items[univ.party[i].race * 2];
			univ.party[i].equip[0] = true;
			univ.party[i].items[1] = start_items[univ.party[i].race * 2 + 1];
			univ.party[i].equip[1] = true;
		}
	// PCs get adjustments
	for (i = 0; i < 6; i++)
		if (univ.party[i].main_status == 1) {
			// Do stat adjs for selected race.
			if (univ.party[i].race == 1)
				univ.party[i].skills[1] += 2;
			if (univ.party[i].race == 2) {
				univ.party[i].skills[0] += 2;
				univ.party[i].skills[2] += 1;
			}
			univ.party[i].max_sp += univ.party[i].skills[9] * 3 + univ.party[i].skills[10] * 3;
			univ.party[i].cur_sp = univ.party[i].max_sp;
		}
	fs::path file = nav_put_party();
	if(!file.empty()) save_party(file);
	party_in_memory = true;
	
	party_in_memory = true;
}

location get_cur_direction(location the_point)
{
	location store_dir;
	
	// This is a kludgy adjustment to adjust for the screen shifting between Exile I & II
	the_point.x += 5;
	the_point.y += 5;
	
	if ((the_point.x < 135) & (the_point.y >= ((the_point.x * 34) / 10) - 293)
		& (the_point.y <= (-1 * ((the_point.x * 34) / 10) + 663)))
		store_dir.x--;
	if ((the_point.x > 163) & (the_point.y <= ((the_point.x * 34) / 10) - 350)
		& (the_point.y >= (-1 * ((the_point.x * 34) / 10) + 721)))
		store_dir.x++;
	
	if ((the_point.y < 167) & (the_point.y <= (the_point.x / 2) + 102)
		& (the_point.y <= (-1 * (the_point.x / 2) + 249)))
		store_dir.y--;
	if ((the_point.y > 203) & (the_point.y >= (the_point.x / 2) + 123)
		& (the_point.y >= (-1 * (the_point.x / 2) + 268)))
		store_dir.y++;
	
	return store_dir;
}

static eDirection find_waterfall(short x, short y, short mode){
	// If more than one waterfall adjacent, randomly selects
	bool to_dir[8];
	for(eDirection i = DIR_N; i < DIR_HERE; i++){
		if(mode == 0){
			to_dir[i] = (scenario.ter_types[univ.town->terrain(x + dir_x_dif[i],y + dir_y_dif[i])].special == TER_SPEC_WATERFALL);
			//printf("%i\n",scenario.ter_types[univ.town->terrain(x + dir_x_dif[i],y + dir_y_dif[i])].flag1);
			if(scenario.ter_types[univ.town->terrain(x + dir_x_dif[i],y + dir_y_dif[i])].flag1.u != i) to_dir[i] = false;
		}else{
			to_dir[i] = (scenario.ter_types[univ.out[x + dir_x_dif[i]][y + dir_y_dif[i]]].special == TER_SPEC_WATERFALL);
			if(scenario.ter_types[univ.out[x + dir_x_dif[i]][y + dir_y_dif[i]]].flag1.u != i) to_dir[i] = false;
		}
	}
	short count = 0;
	for(int i = 0; i < 8; i++)
		count += to_dir[i];
	if(count > 0) count = get_ran(1,1,count);
	else return DIR_HERE;
	for(eDirection i = DIR_N; i < DIR_HERE; i++){
		if(to_dir[i]){
			count--;
			if(count == 0) return i;
		}
	}
	return DIR_HERE; // just in case something wonky happens
}

static void run_waterfalls(short mode){ // mode 0 - town, 1 - outdoors
	short x,y;
	if(mode == 0){
		x = univ.town.p_loc.x;
		y = univ.town.p_loc.y;
	}else{
		x = univ.party.p_loc.x;
		y = univ.party.p_loc.y;
	}
	eDirection dir;
	while((dir = find_waterfall(x,y,mode)) != DIR_HERE){
		add_string_to_buf("  Waterfall!                     ");
		if(mode == 0){
			x += 2 * dir_x_dif[dir];
			y += 2 * dir_y_dif[dir];
			univ.town.p_loc.x += 2 * dir_x_dif[dir];
			univ.town.p_loc.y += 2 * dir_y_dif[dir];
			update_explored(univ.party.p_loc);
		}else{
			x += 2 * dir_x_dif[dir];
			y += 2 * dir_y_dif[dir];
			univ.party.p_loc.x += 2 * dir_x_dif[dir];
			univ.party.loc_in_sec.x += 2 * dir_x_dif[dir];
			univ.party.p_loc.y += 2 * dir_y_dif[dir];
			univ.party.loc_in_sec.y += 2 * dir_y_dif[dir];
			update_explored(univ.party.p_loc);
		}
		draw_terrain();
		print_buf();
		if ((cave_lore_present() > 0) && (get_ran(1,0,1) == 0))
			add_string_to_buf("  (No supplies lost.)");
		else if (univ.party.food > 1800){
			add_string_to_buf("  (Many supplies lost.)");
			univ.party.food -= 50;
		}
		else {
			int n = univ.party.food;
			char s[25];
			univ.party.food = (univ.party.food * 19) / 20;
			sprintf(s,"  (%d supplies lost.)",n - univ.party.food);
			add_string_to_buf(s);
		}
		put_pc_screen();
		play_sound(28);
		pause(8);
	}
	if(mode == 0){
		univ.party.boats[univ.party.in_boat].loc = univ.town.p_loc;
		univ.party.boats[univ.party.in_boat].which_town = univ.town.num;
	}else{
		univ.party.boats[univ.party.in_boat].which_town = 200;
		univ.party.boats[univ.party.in_boat].loc_in_sec = univ.party.loc_in_sec;
		univ.party.boats[univ.party.in_boat].loc = univ.party.p_loc;
		univ.party.boats[univ.party.in_boat].sector.x = univ.party.outdoor_corner.x + univ.party.i_w_c.x;
		univ.party.boats[univ.party.in_boat].sector.y = univ.party.outdoor_corner.y + univ.party.i_w_c.y;
	}
}

bool outd_move_party(location destination,bool forced)
{
	char create_line[60];
	short boat_num,horse_num,spec_num;
	location real_dest, sector_p_in;
	bool keep_going = true,check_f;
	location store_corner,store_iwc;
	ter_num_t ter;
	
	keep_going = check_special_terrain(destination,0,0,&spec_num,&check_f);
	if (check_f == true)
		forced = true;
	if (in_scen_debug && ghost_mode)
		forced = true;
	if (spec_num == 50)
		forced = true;
	
	// If not blocked and not put in town by a special, process move
	if ((keep_going == true) && (overall_mode == MODE_OUTDOORS)) {
		
		real_dest.x = destination.x - univ.party.p_loc.x;
		real_dest.y = destination.y - univ.party.p_loc.y;
		
		sector_p_in.x = univ.party.outdoor_corner.x + univ.party.i_w_c.x;
		sector_p_in.y = univ.party.outdoor_corner.y + univ.party.i_w_c.y;
		
		store_corner = univ.party.outdoor_corner;
		store_iwc = univ.party.i_w_c;
		
		// Check if party moves into new sector
		if ((destination.x < 6) && (univ.party.outdoor_corner.x > 0))
			shift_universe_left();
		if ((destination.x > 90) && (univ.party.outdoor_corner.x < scenario.out_width - 1))
			shift_universe_right();
		if ((destination.y < 6)  && (univ.party.outdoor_corner.y > 0)) {
			shift_universe_up();
		}
		else if ((destination.y > 90)  && (univ.party.outdoor_corner.y < scenario.out_height - 1))
			shift_universe_down();
		// Now stop from going off the world's edge
		real_dest.x = univ.party.p_loc.x + real_dest.x;
		real_dest.y =univ. party.p_loc.y + real_dest.y;
		if ((real_dest.x < 1 /*4*/) && (univ.party.outdoor_corner.x <= 0)) {
			ASB("You've reached the world's edge.");
			return false;
		}
		if (((real_dest.x > 95 /*92*/) && (univ.party.outdoor_corner.x >= scenario.out_width - 2)) ||
			((real_dest.x > 47 /*44*/) && (univ.party.outdoor_corner.x >= scenario.out_width - 1))) {
			ASB("You've reached the world's edge.");
			return false;
		}
		if ((real_dest.y < 1 /*4*/)  && (univ.party.outdoor_corner.y <= 0)) {
			ASB("You've reached the world's edge.");
			return false;
		}
		else if (((real_dest.y > 95 /*92*/)  && (univ.party.outdoor_corner.y >= scenario.out_height - 2)) ||
				 ((real_dest.y > 47 /*44*/)  && (univ.party.outdoor_corner.y >= scenario.out_height - 1))) {
			ASB("You've reached the world's edge.");
			return false;
		}
		
//		if ((store_corner.x != party.outdoor_corner.x) || (store_corner.y != party.outdoor_corner.y) ||
//			(store_iwc.x != party.i_w_c.x) || (store_iwc.y != party.i_w_c.y))
//			clear_map();
		
		
		
//		if (forced == true)
//			for (i = 0; i < 10; i++)
//				if (same_point(destination,party.out_c[i].m_loc) == true)
//					party.out_c[i].exists = false;
		
		ter = univ.out[real_dest.x][real_dest.y];
		if (univ.party.in_boat >= 0) {
			if ((outd_is_blocked(real_dest) == false) //&& (outd_is_special(real_dest) == false)
				// not in towns
				&& ((scenario.ter_types[ter].boat_over == false)
					|| ((real_dest.x != univ.party.p_loc.x) && (real_dest.y != univ.party.p_loc.y)))
				&& (scenario.ter_types[ter].special != TER_SPEC_TOWN_ENTRANCE)) {
				add_string_to_buf("You leave the boat.");
				univ.party.in_boat = -1;
			}
			else if (((real_dest.x != univ.party.p_loc.x) && (real_dest.y != univ.party.p_loc.y))
					 || ((forced == false) && (out_boat_there(destination) < 30)))
				return false;
			else if ((outd_is_blocked(real_dest) == false)
					 && (scenario.ter_types[ter].boat_over == true)
					 && (scenario.ter_types[ter].special != TER_SPEC_TOWN_ENTRANCE)) {
				if(cChoiceDlog("boat-bridge.xml",{"under","land"}).show() == "under")
					forced = true;
				else {
					add_string_to_buf("You leave the boat.             ");
					univ.party.in_boat = -1;
				}
			}
			else if (scenario.ter_types[ter].boat_over == true)
				forced = true;
		}
		
		if (((boat_num = out_boat_there(real_dest)) < 30) && (univ.party.in_boat < 0) && (univ.party.in_horse < 0)) {
			if (flying() == true) {
				add_string_to_buf("You land first.                 ");
				PSD[SDF_PARTY_FLIGHT] = 0;
			}
			give_help(61,0);
			add_string_to_buf("Move: You board the boat.           ");
			univ.party.in_boat = boat_num;
			univ.party.direction = set_direction(univ.party.p_loc, destination);
			
			univ.party.p_loc = real_dest;
			univ.party.i_w_c.x = (univ.party.p_loc.x > 48) ? 1 : 0;
			univ.party.i_w_c.y = (univ.party.p_loc.y > 48) ? 1 : 0;
			univ.party.loc_in_sec = global_to_local(univ.party.p_loc);
			
			if ((store_corner.x != univ.party.outdoor_corner.x) || (store_corner.y != univ.party.outdoor_corner.y) ||
				(store_iwc.x != univ.party.i_w_c.x) || (store_iwc.y != univ.party.i_w_c.y))
				clear_map();
			
			return true;
		}
		else if (((horse_num = out_horse_there(real_dest)) < 30) && (univ.party.in_boat < 0) && (univ.party.in_horse < 0)) {
			if (flying() == true) {
				add_string_to_buf("Land before mounting horses.");
				return false;
			}
			
			give_help(60,0);
			add_string_to_buf("Move: You mount the horses.           ");
			play_sound(84);
			univ.party.in_horse = horse_num;
			univ.party.direction = set_direction(univ.party.p_loc, destination);
			
			univ.party.p_loc = real_dest;
			univ.party.i_w_c.x = (univ.party.p_loc.x > 48) ? 1 : 0;
			univ.party.i_w_c.y = (univ.party.p_loc.y > 48) ? 1 : 0;
			univ.party.loc_in_sec = global_to_local(univ.party.p_loc);
			
			if ((store_corner.x != univ.party.outdoor_corner.x) || (store_corner.y != univ.party.outdoor_corner.y) ||
				(store_iwc.x != univ.party.i_w_c.x) || (store_iwc.y != univ.party.i_w_c.y))
				clear_map();
			
			return true;
		}
		else if ((outd_is_blocked(real_dest) == false) || (forced == true)
				 // Check if can fly over
				 || ((flying() == true) &&
					 (scenario.ter_types[ter].fly_over == true))   ) {
					 if (scenario.ter_types[ter].special == TER_SPEC_DAMAGING || scenario.ter_types[ter].special == TER_SPEC_DANGEROUS) {
						 ASB("Your horses quite sensibly refuse.");
						 return false;
					 }
					 univ.party.direction = set_direction(univ.party.p_loc, destination);
					 
					 if ((flying() == true) && (scenario.ter_types[ter].special == TER_SPEC_TOWN_ENTRANCE)) {
						 add_string_to_buf("Moved: You have to land first.               ");
						 return false;
					 }
					 
					 
					 univ.party.p_loc = real_dest;
					 univ.party.i_w_c.x = (univ.party.p_loc.x > 47) ? 1 : 0;
					 univ.party.i_w_c.y = (univ.party.p_loc.y > 47) ? 1 : 0;
					 univ.party.loc_in_sec = global_to_local(univ.party.p_loc);
					 sprintf ((char *) create_line, "Moved: %s",dir_string[univ.party.direction]);//, univ.party.p_loc.x, univ.party.p_loc.y, univ.party.loc_in_sec.x, univ.party.loc_in_sec.y);
					 add_string_to_buf((char *) create_line);
					 move_sound(univ.out[real_dest.x][real_dest.y],num_out_moves);
					 num_out_moves++;
					 
					 if (univ.party.in_boat >= 0) { // Waterfall!!!
						 run_waterfalls(1);
					 }
					 if (univ.party.in_horse >= 0) {
						 univ.party.horses[univ.party.in_horse].which_town = 200;
						 univ.party.horses[univ.party.in_horse].loc_in_sec = univ.party.loc_in_sec;
						 univ.party.horses[univ.party.in_horse].loc = univ.party.p_loc;
						 univ.party.horses[univ.party.in_horse].sector.x = univ.party.outdoor_corner.x + univ.party.i_w_c.x;
						 univ.party.horses[univ.party.in_horse].sector.y = univ.party.outdoor_corner.y + univ.party.i_w_c.y;
						 
					 }
					 
					 if ((store_corner.x != univ.party.outdoor_corner.x) || (store_corner.y != univ.party.outdoor_corner.y) ||
						 (store_iwc.x != univ.party.i_w_c.x) || (store_iwc.y != univ.party.i_w_c.y))
						 clear_map();
					 
					 return true;
				 }
		else {
			sprintf ((char *) create_line, "Blocked: %s",dir_string[set_direction(univ.party.p_loc, destination)]);
			add_string_to_buf((char *) create_line);
			return false;
		}
	}
	return false;
}

bool town_move_party(location destination,short forced)////
{
	char create_line[60],keep_going = true;
	short boat_there,horse_there,spec_num;
	ter_num_t ter;
	bool check_f = false;
	
	if (in_scen_debug && ghost_mode)
		forced = true;
	
	// remove if not registered
	/*
	if ((scenario.out_width != 3) || (scenario.out_height != 3) ||
		(scenario.num_towns != 21) || (scenario.town_size[3] != 1) || (scenario.town_size[9] != 0)) {
		ASB("Blades of Exile must be registered");
		ASB("before you can play scenarios besides");
		ASB("the unmodified Valley of Dying things.");
		print_buf();
		return false;
	}
	*/
	
	if (monst_there(destination) > univ.town->max_monst())
		keep_going = check_special_terrain(destination,1,0,&spec_num,&check_f);
	if (check_f == true)
		forced = true;
	
	if (spec_num == 50)
		forced = true;
	ter = univ.town->terrain(destination.x,destination.y);
	
	if (keep_going == true) {
		if (univ.party.in_boat >= 0) {
			if ((!is_blocked(destination)) && (!is_special(destination))
				// If to bridge, exit if heading diagonal, keep going if heading horiz or vert
				&& ( (!scenario.ter_types[ter].boat_over)
					|| ((destination.x != univ.town.p_loc.x) && (destination.y != univ.town.p_loc.y)))) {
					add_string_to_buf("You leave the boat.             ");
					univ.party.in_boat = -1;
				}
			else if ((destination.x != univ.town.p_loc.x) && (destination.y != univ.town.p_loc.y))
				return false;
			// Crossing bridge: land or go through
			else if ((!is_blocked(destination)) && (scenario.ter_types[ter].boat_over) && (scenario.ter_types[ter].special == TER_SPEC_BRIDGE)) {
				if(cChoiceDlog("boat-bridge.xml",{"under","land"}).show() == "under")
					forced = true;
				else if (is_blocked(destination) == false) {
					add_string_to_buf("You leave the boat.             ");
					univ.party.in_boat = -1;
				}
			}
			// boat in destination
			else if (town_boat_there(destination) < 30) {
				add_string_to_buf("  Boat there already.             ");
				return false;
			}
			// water or lava
			else if (scenario.ter_types[ter].boat_over == true)
				forced = true;
		}
		
		if (((boat_there = town_boat_there(destination)) < 30) && (univ.party.in_boat < 0)) {
			if (univ.party.boats[boat_there].property == true) {
				add_string_to_buf("  Not your boat.             ");
				return false;
			}
			give_help(61,0);
			add_string_to_buf("Move: You board the boat.           ");
			univ.party.in_boat = boat_there;
			univ.party.direction = set_direction(univ.town.p_loc, destination);
			
			univ.town.p_loc = destination;
			center = univ.town.p_loc;
			
			return true;
		}
		else if (((horse_there = town_horse_there(destination)) < 30) && (univ.party.in_horse < 0)) {
			if (univ.party.horses[horse_there].property == true) {
				add_string_to_buf("  Not your horses.             ");
				return false;
			}
			give_help(60,0);
			add_string_to_buf("Move: You mount the horses.           ");
			play_sound(84);
			univ.party.in_horse = horse_there;
			univ.party.direction = set_direction(univ.town.p_loc, destination);
			
			univ.town.p_loc = destination;
			center = univ.town.p_loc;
			
			return true;
		}
		else if ((is_blocked(destination) == false) || (forced == 1)) {
			if (univ.party.in_horse >= 0) {
				if (scenario.ter_types[ter].special == TER_SPEC_DAMAGING) {
					ASB("Your horses quite sensibly refuse.");
					return false;
				}
				if (scenario.ter_types[ter].block_horse == true) {
					ASB("You can't take horses there!");
					return false;
				}
				if ((univ.town->lighting_type > 0) && (get_ran(1,0,1) == 0)) {
					ASB("The darkness spooks your horses.");
					return false;
				}
				
			}
			univ.party.direction = set_direction(univ.town.p_loc, destination);
			univ.town.p_loc = destination;
			sprintf ((char *) create_line, "Moved: %s",dir_string[univ.party.direction]);
			add_string_to_buf((char *) create_line);
//			place_treasure(destination,5,3);
			
			move_sound(univ.town->terrain(destination.x,destination.y),(short) univ.party.age);
			
			if (univ.party.in_boat >= 0) {
				// Waterfall!!!
				run_waterfalls(0);
			}
			if (univ.party.in_horse >= 0) {
				univ.party.horses[univ.party.in_horse].loc = univ.town.p_loc;
				univ.party.horses[univ.party.in_horse].which_town = univ.town.num;
			}
			center = univ.town.p_loc;
			return true;
		}
		else {
			if (is_door(destination) == true)
				sprintf ((char *) create_line, "Door locked: %s               ",dir_string[set_direction(univ.town.p_loc, destination)]);
			else sprintf ((char *) create_line, "Blocked: %s               ",dir_string[set_direction(univ.town.p_loc, destination)]);
			add_string_to_buf((char *) create_line);
			return false;
		}
	}
	return false;
}



bool someone_poisoned()
{
	short i;
	
	for (i = 0; i < 6; i++)
		if ((univ.party[i].main_status == 1) && (univ.party[i].status[2] > 0))
			return true;
	return false;
}

short nearest_monster()
{
	short i = 100,j,s;
	
	for (j = 0; j < 10; j++)
		if (univ.party.out_c[j].exists == true) {
			s = dist(univ.party.p_loc,univ.party.out_c[j].m_loc);
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
	return (scenario.out_width * (univ.party.outdoor_corner.y + univ.party.i_w_c.y) + univ.party.outdoor_corner.x + univ.party.i_w_c.x);
}

short count_walls(location loc) // TODO: Generalize this function
{
	unsigned char walls[31] = {5,6,7,8,9, 10,11,12,13,14, 15,16,17,18,19, 20,21,22,23,24,
		25,26,27,28,29, 30,31,32,33,34, 35};
	short answer = 0;
	short k = 0;
	
	for (k = 0; k < 31 ; k++) {
		if (univ.out[loc.x + 1][loc.y] == walls[k])
			answer++;
		if (univ.out[loc.x - 1][loc.y] == walls[k])
			answer++;
		if (univ.out[loc.x][loc.y + 1] == walls[k])
			answer++;
		if (univ.out[loc.x][loc.y - 1] == walls[k])
			answer++;
	}
	return answer;
}

bool is_sign(ter_num_t ter)
{
	
	if (scenario.ter_types[ter].special == TER_SPEC_IS_A_SIGN)
		return true;
	return false;
}

bool check_for_interrupt(){
	using kb = sf::Keyboard;
#ifdef __APPLE__
	if((kb::isKeyPressed(kb::LSystem) || kb::isKeyPressed(kb::RSystem)) && kb::isKeyPressed(kb::Period))
		return true;
#endif
	if((kb::isKeyPressed(kb::LControl) || kb::isKeyPressed(kb::RControl)) && kb::isKeyPressed(kb::C))
		return true;
	return false;
}
