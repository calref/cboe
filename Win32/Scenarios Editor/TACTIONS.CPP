#include <Windows.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include "Global.h"
#include "graphics.h"
#include "tactions.h"
#include "edsound.h"
#include "scenario.h"
#include "tfileio.h"
#include "keydlgs.h"
#include "townout.h"
#include "graphutl.h"
#include "buttonmg.h"


RECT world_screen;
	// border rects order: top, left, bottom, right //
RECT border_rect[4];
short current_block_edited = 0;
short current_terrain_type = 0;
short safety = 0;
location spot_hit,last_spot_hit = {-1,-1};
creature_start_type null_monst = {0,0,{0,0},0,0,0,0,0,0};
//creature_start_type store_monst = {0,0,{0,0},0,0,0,0,0,0};
Boolean sign_error_received = FALSE;
short copied_spec = -1;

Boolean good_palette_buttons[6][8] = {
									{1,1,1,1,1,1,1,1},
									{1,1,1,1,1,1,0,1},
									{1,1,1,1,1,1,1,1},
									{1,1,1,0,1,1,1,1},
									{1,1,1,1,1,1,0,1},
									{1,1,1,1,1,1,1,1}};
preset_item_type store_place_item = {{0,0},-1,0,0,0,0,0};

short flood_count = 0;

RECT terrain_rects[256],terrain_rect_base = {0,0,16,16},command_rects[21];							


extern short ulx,uly,cen_x, cen_y, overall_mode,cur_town;
extern Boolean mouse_button_held,editing_town;
extern short cur_viewing_mode;
extern town_record_type town;
extern big_tr_type t_d;
extern short town_type;  // 0 - big 1 - ave 2 - small
extern short max_dim[3],mode_count,to_create;
extern unsigned char template_terrain[64][64];
extern short_item_record_type item_list[400];
extern scenario_data_type scenario;
extern HWND right_sbar,mainPtr;
extern outdoor_record_type current_terrain;
extern location cur_out;
extern piles_of_stuff_dumping_type *data_store;
Boolean small_any_drawn = FALSE;
extern talking_record_type talking;
extern Boolean change_made;
extern scen_item_data_type scen_item_list;
extern piles_of_stuff_dumping_type *data_store;
extern char scen_strs[160][256];
extern char scen_strs2[110][256];
extern char talk_strs[170][256];
extern char town_strs[180][256];

RECT left_buttons[NLS][2]; // 0 - whole, 1 - blue button
short left_button_status[NLS]; // 0 - clear, 1 - text, 2 - title text, +10 - button
short right_button_status[NRS]; 
RECT right_buttons[NRSONPAGE];
RECT palette_buttons[8][6];
short current_rs_top = 0;
	creature_start_type last_placed_monst = {0,0,{0,0},0,0,0,0,0,0,0};

special_node_type null_spec_node = {0,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
talking_node_type null_talk_node = {0,0,{120,120,120,120},{120,120,120,120},{0,0,0,0}};

RECT working_rect;
location last_space_hit;
Boolean erasing_mode;
unsigned char current_ground = 0;

short special_to_paste = -1;

void init_screen_locs()
{
	int i,j,q,r;
	
	for (i = 0; i < 4; i++)
		border_rect[i] = world_screen;
	border_rect[0].bottom = border_rect[0].top + 8;
	border_rect[1].right = border_rect[1].left + 8;
	border_rect[2].top = border_rect[2].bottom - 8;
	border_rect[3].left = border_rect[3].right - 8;
		
	for (i = 0; i < 256; i++) {
		terrain_rects[i] = terrain_rect_base;
		OffsetRect(&terrain_rects[i],3 + (i % 17) * (terrain_rect_base.right + 1),
			3 + (i / 17) * (terrain_rect_base.bottom + 1));
		}
}

Boolean handle_action(POINT the_point, WPARAM wparam, LPARAM lparam )
{
	short i,j, x, y;
	Boolean are_done = FALSE;
	char str[256],s2[256];
	
	RECT targ_rect;	
	Boolean need_redraw = FALSE,option_hit = FALSE;
	location spot_hit;
	POINT cur_point,cur_point2;
	short right_top,right_hit;
	short old_mode;
	RECT temp_rect;
	long dummy;
	
	Boolean to_return = FALSE;
	Boolean ctrl_key = FALSE;
	Boolean right_button = FALSE;

	if (lparam == -2)
		option_hit = right_button = TRUE;
	if (lparam != -1) {
		the_point.x -= ulx;
      the_point.y -= uly;
		}
	if (MK_CONTROL & wparam)
		option_hit = ctrl_key = TRUE;
		
	for (i = 0; i < NLS; i++)
		if ((mouse_button_held == FALSE) && (PtInRect ( &left_buttons[i][0],the_point) )
			&& (left_button_status[i] >= 10))  {
			draw_lb_slot(i,1);
			play_sound(37);
			Delay(10,&dummy);
			draw_lb_slot(i,0);
			if (overall_mode == 61) {
				switch(i)
					{
						case 3: // new
							build_scenario();
							if (overall_mode == 60)
								set_up_main_screen();
							break;
	
						case 4: // edit
							load_scenario();
							if (overall_mode == 60) {
								set_up_main_screen();
								update_item_menu();
								}
							break;
					}
				}
			else if ((overall_mode == 60) || (overall_mode == 62)) {
				switch(i)
					{
						case 0:
							break;
	
						case 1:
							break;
						case 2: // start terrain editing
							start_terrain_editing();
							break;
						case 3: // start terrain editing
							start_monster_editing(0);
							break;
						case 4: // start item editing
							start_item_editing(0);
							break;
						case 5: // new town
							if (change_made == TRUE) {
								give_error("You need to save the changes made to your scenario before you can add a new town.",
									"",0);
								return are_done;
								}
							if (scenario.num_towns >= 200) {
								give_error("You have reached the limit of 200 towns you can have in one scenario.",
									"",0);
								return are_done;
								}
							if (new_town(scenario.num_towns) == TRUE)
								set_up_main_screen();
							break;
						case 6:
							SetScrollPos(right_sbar,SB_CTL,0,TRUE);
							start_string_editing(0,0);
							break;
						case 7: 
							start_special_item_editing();
							break;
						case 11: // pick out
							if (change_made == TRUE) {
								if (save_check(859) == FALSE)
									break;
								}
							x = pick_out(cur_out);
							if (x >= 0) {
								spot_hit.x = x / 100;
								spot_hit.y = x % 100;
								load_outdoors(spot_hit,0);
								augment_terrain(spot_hit);
								set_up_main_screen();
								}
							break;
						case 12: // edit outdoors
							start_out_edit();
							mouse_button_held = FALSE;
							return FALSE;
							break; 
						case 16: // pick town
							if (change_made == TRUE) {
								if (save_check(859) == FALSE)
									break;							
								}
							x = pick_town_num(855,cur_town);
							if (x >= 0) {
								load_town(x);
								set_up_main_screen();
							start_town_edit();
							mouse_button_held = FALSE;
							return FALSE;
								}
							break;
						case 17: // edit town
							start_town_edit();
							mouse_button_held = FALSE;
							return FALSE;
							break; 
						case 18: 
							start_dialogue_editing(0);
							break;

					}
				}
			if ((overall_mode < 60) && (i == NLS - 1)) {
				set_cursor(0);
				set_up_main_screen();
				}
		mouse_button_held = FALSE;
		}
	
	if (overall_mode == 60) {
		right_top = GetScrollPos(right_sbar,SB_CTL);
		for (i = 0; i < NRSONPAGE; i++)
			if ((mouse_button_held == FALSE) && (PtInRect (&right_buttons[i],the_point) )
				&& (right_button_status[i + right_top] > 0))  {
			
				right_hit = right_button_status[i + right_top];
				j = right_hit % 1000;
				//flash_rect(left_buttons[i][0]);
				draw_rb_slot(i + right_top,1);
				play_sound(37);
				Delay(10,&dummy);
				draw_rb_slot(i + right_top,0);
				change_made = TRUE;
				switch(right_hit / 1000)
				{
					case 1:
						edit_ter_type(j);
						update_item_menu();
						break;
					case 2:
						edit_monst_type(j); 
						update_item_menu();
						start_monster_editing(1);
						break;
					case 3: 
						edit_item_type(j);
						update_item_menu();
						start_item_editing(1);
						break;
					case 4:
						if (option_hit == TRUE) {
							scenario.scen_specials[j] = null_spec_node;
							} 
							else edit_spec_enc(j,0,0); 
						start_special_editing(0,1);
						break;
					case 5: 
						if (option_hit == TRUE) {
							current_terrain.specials[j] = null_spec_node;
							} 
							else edit_spec_enc(j,1,0); 
						start_special_editing(1,1);
						break;
					case 6: 
					if (option_hit == TRUE) {
							town.specials[j] = null_spec_node;
							} 
							else edit_spec_enc(j,2,0); 
						start_special_editing(2,1);
						break;
					case 7:
						if (option_hit == TRUE) {
							get_str(s2,35,j + 1);
							if (j < 160)
								strcpy(scen_strs[j],(char *) s2);
								else strcpy(scen_strs2[j - 160],(char *) s2);
							} 
							else edit_text_str(j,0);
						start_string_editing(0,1);
						break;

					case 8:
						if (option_hit == TRUE) {
							get_str(s2,37,j + 1);
							strcpy(data_store->out_strs[j],(char *) s2);
							} 
							else edit_text_str(j,1);
						start_string_editing(1,1);
						break;
					case 9:
						if (option_hit == TRUE) {
							get_str(s2,36,j + 1);
							strcpy(town_strs[j],(char *) s2);
							} 
							else edit_text_str(j,2);
						start_string_editing(2,1);
						break;
					case 10:
						edit_spec_item(j);
						start_special_item_editing();
					break;
					case 12:
						edit_talk_node(j,0);
						start_dialogue_editing(1);
						break;
					case 13:
						edit_basic_dlog(j);
						start_dialogue_editing(1);
						break;
				}
		//draw_rb_slot(i + right_top,0);
		mouse_button_held = FALSE;
		}
		}
	if ((overall_mode < 60) //&& (PtInRect (the_point, &world_screen))
		&& (the_point.x > world_screen.left + 8) && (the_point.x < world_screen.right - 8)
			&& (the_point.y > world_screen.top + 8) && (the_point.y < world_screen.bottom - 8) ) {	
		if (cur_viewing_mode == 0) {	
			i = (the_point.x - TER_RECT_UL_X - 8) / 28;
			j = (the_point.y - TER_RECT_UL_Y - 8) / 36;
		
			spot_hit.x = cen_x + i - 4;
			spot_hit.y = cen_y + j - 4;
			if ((i < 0) || (i > 8) || (j < 0) || (j > 8))
				spot_hit.x = -1;
			}
			else {
				
				i = (the_point.x - TER_RECT_UL_X - 8) / 4;
				j = (the_point.y - TER_RECT_UL_Y - 8) / 4;
				spot_hit.x = i; spot_hit.y = j;
				}
		
		if ((mouse_button_held == TRUE) && (spot_hit.x == last_spot_hit.x) &&
			(spot_hit.y == last_spot_hit.y))
			return are_done;
			else last_spot_hit = spot_hit;
		if (mouse_button_held == FALSE)
			last_spot_hit = spot_hit;
		
		old_mode = overall_mode;
		change_made = TRUE;
	
		if ((spot_hit.x < 0) || (spot_hit.x > ((editing_town == TRUE) ? max_dim[town_type] - 1 : 47)) ||
			(spot_hit.y < 0) || (spot_hit.y > ((editing_town == TRUE) ? max_dim[town_type] - 1 : 47))) ;
		else switch (overall_mode) {
			case 0:
			 if (((mouse_button_held == FALSE) && 
			 (((editing_town == TRUE) && (t_d.terrain[spot_hit.x][spot_hit.y] == current_terrain_type)) ||
			 ((editing_town == FALSE) && (current_terrain.terrain[spot_hit.x][spot_hit.y] == current_terrain_type)) ||
			 ((is_mountain(spot_hit.x,spot_hit.y)) && (current_terrain_type >= 22)
			 	 && (current_terrain_type <= 35) && (current_terrain_type != 23)) ||
			 ((is_erasable_water(spot_hit.x,spot_hit.y)) && (current_terrain_type >= 50)&& (current_terrain_type <= 62)) ||
			 ((is_correctable_wall((short) spot_hit.x,(short) spot_hit.y)) 
			 	&& (current_terrain_type >= 5) && (current_terrain_type <= 21) && (scenario.ter_types[current_terrain_type].blockage == 5)) ||
			 ((is_hill(spot_hit.x,spot_hit.y)) && (current_terrain_type >= 36) && (current_terrain_type <= 49)
			 	&& (current_terrain_type != 37)))) ||
			 	((mouse_button_held == TRUE) && (erasing_mode == TRUE))) {
			 	set_terrain(spot_hit,current_ground);
				set_cursor(0);
				erasing_mode = TRUE;
				mouse_button_held = TRUE;
				}
					else 
					{
					mouse_button_held = TRUE;
					set_cursor(0);
			 		set_terrain(spot_hit,current_terrain_type);
					erasing_mode = FALSE;
					}
			break;
			
			case 1: case 3: case 9: case 35: case 36:
				if (mouse_button_held == TRUE)
					break;
				if (mode_count == 2) {
					working_rect.left = spot_hit.x;
				 	working_rect.top = spot_hit.y;
					mode_count = 1;
							set_cursor(6);
					set_string("Now select lower right corner","");
					break;
					}
				working_rect.right = spot_hit.x;
			 	working_rect.bottom = spot_hit.y;			
						if ((overall_mode == 1) || (overall_mode == 36)) {
							change_rect_terrain(working_rect,current_terrain_type,20,0);
							}
						else if (overall_mode == 35) {
							change_rect_terrain(working_rect,current_terrain_type,20,1);
							}
							else if (overall_mode == 9) {
								town.in_town_rect = working_rect;
								}
							else {
								if (editing_town == TRUE) {
									for (x = 0; x < 16; x++)
									if (t_d.room_rect[x].right == 0) {
										t_d.room_rect[x] = working_rect;
										sprintf(town_strs[x + 1],"");
										if (edit_area_rect_str(x,1) == FALSE)
											t_d.room_rect[x].right = 0;
										x = 500;
										}
									}
								if (editing_town == FALSE) {
									for (x = 0; x < 8; x++)
									if (current_terrain.info_rect[x].right == 0) {
										current_terrain.info_rect[x] = working_rect;
										sprintf(data_store->out_strs[x + 1],"");
										if (edit_area_rect_str(x,0) == FALSE)
											current_terrain.info_rect[x].right = 0;
										x = 500;
										}
									
									}
								if (x < 500)
									give_error("You have placed the maximum number of area rectangles (16 in town, 8 outdoors).","",0);
								}
				overall_mode = 0;
				set_cursor(0);
				break;
			case 2:					
				if (mouse_button_held == TRUE)
					break;
				if (editing_town == FALSE) {
					current_terrain.wandering_locs[mode_count - 1].x = spot_hit.x;
					current_terrain.wandering_locs[mode_count - 1].y = spot_hit.y;
					}
				if (editing_town == TRUE) {
					town.wandering_locs[mode_count - 1].x = spot_hit.x;
					town.wandering_locs[mode_count - 1].y = spot_hit.y;
					}
				mode_count--;
				switch (mode_count) {	
					case 3: set_string("Place second wandering monster arrival point","");break;
					case 2: set_string("Place third wandering monster arrival point","");break;
					case 1: set_string("Place fourth wandering monster arrival point","");break;
					case 0:
						overall_mode = 0;
						set_cursor(0);
						set_string("Drawing mode","");
						break;
					}
				break;
			case 6:
				break;
				
			case 29:
				mouse_button_held = TRUE;
				change_circle_terrain(spot_hit,4,current_terrain_type,20);
				break;
			case 30:
				mouse_button_held = TRUE;
				change_circle_terrain(spot_hit,1,current_terrain_type,20);
				break;
			case 31:
				mouse_button_held = TRUE;
				shy_change_circle_terrain(spot_hit,4,current_terrain_type,1);
				break;
			case 32:
				mouse_button_held = TRUE;
				shy_change_circle_terrain(spot_hit,2,current_terrain_type,1);
				break;
			case 37: // erase
				change_circle_terrain(spot_hit,2,current_ground,20);
				mouse_button_held = TRUE;
				break;
			case 4:
				for (x = 0; x < 64; x++) 
					if (town.preset_items[x].item_code < 0) {
						town.preset_items[x].item_loc.x = spot_hit.x;
						town.preset_items[x].item_loc.y = spot_hit.y;
						town.preset_items[x].item_code = mode_count;
						town.preset_items[x].ability = -1;
						if ((scen_item_list.scen_items[mode_count].variety == 3) ||
							(scen_item_list.scen_items[mode_count].variety == 11))
								town.preset_items[x].ability = get_ran(1,4,6);
						//town.preset_items[x].charges = 0;
						town.preset_items[x].always_there = 0;
						town.preset_items[x].property = 0;
						town.preset_items[x].contained = container_there(town.preset_items[x].item_loc);
						store_place_item = town.preset_items[x];
						x = 70;
						}
				if (x == 64) 
					give_error("You can only have 64 preset items in each town.","",0);
					
				overall_mode = 0;
				set_cursor(0);
				set_string("Drawing mode","");
				break;
			case 5:
				for (x = 0; x < 64; x++) 
					if ((spot_hit.x == town.preset_items[x].item_loc.x) &&
					(spot_hit.y == town.preset_items[x].item_loc.y) && (town.preset_items[x].item_code >= 0)) {			
						edit_placed_item(x);
						store_place_item = town.preset_items[x];
						}
				overall_mode = 0;
				set_cursor(0);
				break;
			case 7:
				overall_mode = 0;
				set_cursor(0);
				break;
			case 8:
				if (last_placed_monst.number == 0) {
					give_error("Either no monster has been placed, or the last time you tried to place a monster the operation failed.","",0);
					break;
					}
				for (i = 0; i < 60; i++)
					if (t_d.creatures[i].number == 0) {
						t_d.creatures[i] = last_placed_monst;
						t_d.creatures[i].start_loc = spot_hit;
						if ((i >= 30) && (scenario.town_size[cur_town] == 2)) {
							give_error("Small towns can have at most 30 preset monsters.","",0); // error
							t_d.creatures[i].number = 0;
							}
						else if ((i >= 40) && (scenario.town_size[cur_town] == 1)) {
							give_error("Medium towns can have at most 40 preset monsters.","",0); // error
							t_d.creatures[i].number = 0;
							}
						i = 60;	
						}
				overall_mode = 0;
				set_cursor(0);
				break;					
			case 28:
				for (i = 0; i < 60; i++)
					if (t_d.creatures[i].number == 0) {
						t_d.creatures[i].start_loc = spot_hit;
						t_d.creatures[i].number = mode_count;
						t_d.creatures[i].start_attitude =
							scenario.scen_monsters[mode_count].default_attitude;
						t_d.creatures[i].mobile = 1;
						t_d.creatures[i].time_flag = 0;
						t_d.creatures[i].extra1 = 0;
						t_d.creatures[i].extra2 = 0;
						t_d.creatures[i].spec1 = -1;
						t_d.creatures[i].spec2 = -1;
						t_d.creatures[i].spec_enc_code = 0;
						t_d.creatures[i].time_code = 0;
						t_d.creatures[i].monster_time = 0;
						t_d.creatures[i].personality = -1;
						t_d.creatures[i].special_on_kill = -1;
						t_d.creatures[i].facial_pic = scenario.scen_monsters[mode_count].default_facial_pic;
						if ((i >= 30) && (scenario.town_size[cur_town] == 2)) {
							give_error("Small towns can have at most 30 preset monsters.","",0); // error
							t_d.creatures[i].number = 0;
							}
						else if ((i >= 40) && (scenario.town_size[cur_town] == 1)) {
							give_error("Medium towns can have at most 40 preset monsters.","",0); // error
							t_d.creatures[i].number = 0;
							}
						last_placed_monst = t_d.creatures[i];
							
						i = 70;	
						}
				if ((i < 70) && (scenario.town_size[cur_town] == 0)) {
					give_error("Large towns can have at most 60 preset monsters.","",0); // error
					}
				overall_mode = 0;
				set_cursor(0);
				break;

			case 10: case 11: case 12: case 13:
				town.start_locs[overall_mode - 10].x = spot_hit.x; 
				town.start_locs[overall_mode - 10].y = spot_hit.y; 
				set_cursor(0);
				overall_mode = 0;
				break;
			case 20:
				make_web(spot_hit.x,spot_hit.y); overall_mode = 0; break;
			case 21:
				make_crate(spot_hit.x,spot_hit.y); overall_mode = 0; break;
			case 22:
				make_barrel(spot_hit.x,spot_hit.y); overall_mode = 0; break;
			case 23:
				make_fire_barrier(spot_hit.x,spot_hit.y); overall_mode = 0; break;
			case 24:
				make_force_barrier(spot_hit.x,spot_hit.y); overall_mode = 0; break;
			case 25:
				make_quickfire(spot_hit.x,spot_hit.y); overall_mode = 0; break;

			case 26:
				take_quickfire(spot_hit.x,spot_hit.y); 
				take_force_barrier(spot_hit.x,spot_hit.y); 
				take_fire_barrier(spot_hit.x,spot_hit.y); 
				take_barrel(spot_hit.x,spot_hit.y); 
				take_crate(spot_hit.x,spot_hit.y); 
				take_web(spot_hit.x,spot_hit.y); 
				for (i = 0; i < 8; i++)
					take_sfx(spot_hit.x,spot_hit.y,i);
				set_cursor(0);
				overall_mode = 0;
				break;
			case 33:
				make_sfx(spot_hit.x,spot_hit.y,mode_count); overall_mode = 0; break;
			case 34:
				if (editing_town == TRUE)
					set_new_terrain(t_d.terrain[spot_hit.x][spot_hit.y]);
					else set_new_terrain(current_terrain.terrain[spot_hit.x][spot_hit.y]);
				set_cursor(0);
				overall_mode = 0;
				break;
			case 46:
				if (store_place_item.item_code < 0) {
					give_error("Either no item has been placed, or the last time you tried to place an item the operation failed.","",0);
					break;
					}
				for (x = 0; x < 64; x++) 
					if (town.preset_items[x].item_code < 0) {
						town.preset_items[x] = store_place_item;
						town.preset_items[x].item_loc.x = spot_hit.x;
						town.preset_items[x].item_loc.y = spot_hit.y;

						town.preset_items[x].contained = container_there(town.preset_items[x].item_loc);
						x = 64;
						}
				set_cursor(0);
				overall_mode = 0;
				break;
			case 38: //edit sign
				if (editing_town == TRUE) {
					for (x = 0; x < 15; x++)
						if ((town.sign_locs[x].x == spot_hit.x) && (town.sign_locs[x].y == spot_hit.y)) {
							edit_sign(x,scenario.ter_types[t_d.terrain[spot_hit.x][spot_hit.y]].picture);
							x = 30;
							}
					if (x == 15) {
						give_error("Either this space is not a sign, or you have already placed too many signs on this level.",
							"",0);
						}	
					}
				if (editing_town == FALSE) {
					for (x = 0; x < 8; x++)
						if ((current_terrain.sign_locs[x].x == spot_hit.x) && (current_terrain.sign_locs[x].y == spot_hit.y)) {
							edit_sign(x,scenario.ter_types[current_terrain.terrain[spot_hit.x][spot_hit.y]].picture);
							x = 30;
							}
					if (x == 8) {
						give_error("Either this space is not a sign, or you have already placed too many signs on this level.",
							"",0);
						}	
					}
				set_cursor(0);
				overall_mode = 0;
				break;
			case 39: //edit monst
				for (x = 0; x < 60; x++) 
					if (monst_on_space(spot_hit,x) == TRUE) {
						edit_placed_monst(x);
						last_placed_monst = t_d.creatures[x];
						}
				set_cursor(0);
				overall_mode = 0;
				break;
			case 40: //make special
				place_edit_special(spot_hit);
				set_cursor(0);
				overall_mode = 0;
				break;
			case 41: //copy special
				if (editing_town == TRUE) {
					for (x = 0; x < 50; x++)
						if ((town.special_locs[x].x == spot_hit.x) && (town.special_locs[x].y == spot_hit.y)) {
							copied_spec = town.spec_id[x];
							x = 500;
							}
					}
				if (editing_town == FALSE) {
					for (x = 0; x < 18; x++)
						if ((current_terrain.special_locs[x].x == spot_hit.x) && (current_terrain.special_locs[x].y == spot_hit.y)) {
							copied_spec = current_terrain.special_id[x];
							x = 500;
							}
					}
				if (x < 500)
					give_error("There wasn't a special on that spot.","",0);
				set_cursor(0);
				overall_mode = 0;
				break;
			case 42: //paste special
				if (copied_spec < 0) {
					give_error("You need to select a special to copy first.","",0);
					break;
					}
				if (editing_town == TRUE) {
					for (x = 0; x < 50; x++)
						if (town.special_locs[x].x == 100) {
							town.special_locs[x] = spot_hit;
							town.spec_id[x] = copied_spec ;
							x = 500;
							}
					}
				if (editing_town == FALSE) {
					if ((spot_hit.x == 0) || (spot_hit.x == 47) || (spot_hit.y == 0) || (spot_hit.y == 47)) {
						fancy_choice_dialog(870,0);
						break;
						}
					for (x = 0; x < 18; x++)
						if (current_terrain.special_locs[x].x == 100) {
							current_terrain.special_locs[x] = spot_hit;
							current_terrain.special_id[x] = copied_spec ;
							x = 500;
							}
					}
				
				if (x < 500)
					give_error("Each town can have at most 50 locations with special encounters. Each outdoor section can have at most 18. You'll need to erase some special spaces before you can place more.","",0);

				set_cursor(0);
				overall_mode = 0;
				break;
			case 43: //erase special
				if (editing_town == TRUE) {
					for (x = 0; x < 50; x++)
						if ((town.special_locs[x].x == spot_hit.x) && (town.special_locs[x].y == spot_hit.y)) {
							town.special_locs[x].x = 100;
							x = 500;
							}
					}
				if (editing_town == FALSE) {
					for (x = 0; x < 18; x++)
						if ((current_terrain.special_locs[x].x == spot_hit.x) && (current_terrain.special_locs[x].y == spot_hit.y)) {
							current_terrain.special_locs[x].x = 100;
							x = 500;
							}
					}
				set_cursor(0);
				overall_mode = 0;
				break;
			case 44: //edit special
				set_special(spot_hit);
				set_cursor(0);
				overall_mode = 0;
				break;
			case 45: //edit town entry
				town_entry(spot_hit);
				overall_mode = 0;
				set_cursor(0);
				break;
			case 47: //edit out start loc
				if (editing_town == TRUE) {
								set_string("You must be outdoors to do this...","");
								break;
								}
				if (fancy_choice_dialog(864,0) == 2)
					break;
				if ((spot_hit.x != minmax(4,43,spot_hit.x)) ||
					(spot_hit.y != minmax(4,43,spot_hit.y))) {
						give_error("You can't put the starting location this close to the edge of an outdoor section. It has to be at least 4 spaces away.","",0);
						break;
						}
				scenario.out_sec_start.x = cur_out.x;
				scenario.out_sec_start.y = cur_out.y;
				scenario.out_start = spot_hit;
				set_cursor(0);
				overall_mode = 0;
				break;
			case 48: //delete monst
				for (x = 0; x < 60; x++) 
					if (monst_on_space(spot_hit,x) == TRUE) {
						t_d.creatures[x].number = 0;
						t_d.creatures[x].start_loc.x = 0;
						t_d.creatures[x].start_loc.y = 0;
						}
				set_cursor(0);
				overall_mode = 0;
				break;
			case 49: // delete item
				for (x = 0; x < 64; x++) 
					if ((spot_hit.x == town.preset_items[x].item_loc.x) &&
					(spot_hit.y == town.preset_items[x].item_loc.y) && (town.preset_items[x].item_code >= 0)) {
						town.preset_items[x].item_code = -1;
						}
				set_cursor(0);
				overall_mode = 0;
				break;
			}
			if ((overall_mode == 0) && (old_mode != 0))
				set_string("Drawing mode","");
			draw_terrain();

		}
	if (overall_mode < 60) {
		if ((PtInRect (&border_rect[0],the_point )) & (cen_y > ((editing_town == TRUE) ? 4 : 3))) {
			cen_y--;
			if (ctrl_key == TRUE)
				cen_y = ((editing_town == TRUE) ? 4 : 3);
			need_redraw = TRUE;
			mouse_button_held = TRUE;
			}
		if ((PtInRect (&border_rect[1],the_point )) & (cen_x > ((editing_town == TRUE) ? 4 : 3))) {
			cen_x--;
			if (ctrl_key == TRUE)
				cen_x = ((editing_town == TRUE) ? 4 : 3);		
			need_redraw = TRUE;
			mouse_button_held = TRUE;
			}
		if ((PtInRect (&border_rect[2],the_point )) & (cen_y < ((editing_town == TRUE) ? max_dim[town_type] - 5 : 44))) {
			cen_y++;
			if (ctrl_key == TRUE)
				cen_y = (editing_town == TRUE) ? max_dim[town_type] - 5 : 44;		
			need_redraw = TRUE;
			mouse_button_held = TRUE;
			}
		if ((PtInRect (&border_rect[3],the_point )) & (cen_x < ((editing_town == TRUE) ? max_dim[town_type] - 5 : 44))) {
			cen_x++;
			if (ctrl_key == TRUE)
				cen_x = (editing_town == TRUE) ? max_dim[town_type] - 5 : 44;
			need_redraw = TRUE;
			mouse_button_held = TRUE;
			}
		if (need_redraw == TRUE) {
			draw_terrain();
			place_just_location();
			need_redraw = FALSE;
			}
		}

	if ((mouse_button_held == FALSE) && ((overall_mode < 60) || (overall_mode == 62))) {
		cur_point = the_point;
		cur_point.x -= RIGHT_AREA_UL_X;
		cur_point.y -= RIGHT_AREA_UL_Y;
		
		for (i = 0; i < 256; i++)
			if (PtInRect(&terrain_rects[i],cur_point)) {
				temp_rect = terrain_rects[i];
				OffsetRect(&temp_rect,RIGHT_AREA_UL_X, RIGHT_AREA_UL_Y );
				flash_rect(temp_rect);
				if (overall_mode < 60) {
					set_new_terrain(i);
					}
					else {
						edit_ter_type(i);
						set_up_terrain_buttons();
						}
				place_location();
				}
		cur_point2 = cur_point;
		cur_point2.x -= 5;
		cur_point2.y -= terrain_rects[255].bottom + 5;
		for (i = 0; i < 8; i++)
			for (j = 0; j < 6; j++) {
			if ((good_palette_buttons[j][i] > 0) && (mouse_button_held == FALSE) && (PtInRect(&palette_buttons[i][j],cur_point2))
				&& ((j < 3) || (editing_town == TRUE)) && (overall_mode < 60)) {
					temp_rect = palette_buttons[i][j];
					OffsetRect(&temp_rect,RIGHT_AREA_UL_X + 5, RIGHT_AREA_UL_Y + terrain_rects[255].bottom + 5);
					flash_rect(temp_rect);
					switch (i + 100 * j) {
						case 0:
							set_string("Drawing mode"," ");
							overall_mode = 0;
							set_cursor(0);
							break;
						case 1: 
							set_string("Paintbrush (large)","");
							overall_mode = 28 + i;
							set_cursor(2);
							break;
						case 2: 
							set_string("Paintbrush (small)","");
							set_cursor(2);
							overall_mode = 28 + i;
							break;
						case 3: 
							set_string("Spraycan (large)","");
							set_cursor(3);
							overall_mode = 28 + i;
							break;
						case 4: 
							set_string("Spraycan (small)","");
							set_cursor(3);
							overall_mode = 28 + i;
							break;
						case 5: 
							set_string("Eyedropper","Select terrain to draw");
							set_cursor(1);
							overall_mode = 34;
							break;
						case 6: 
							overall_mode = 35;
							mode_count = 2;
							set_cursor(5);
							set_string("Fill rectangle (hollow)","Select upper left corner");
							break;
						case 7:
							overall_mode = 36;
							mode_count = 2;
							set_cursor(5);
							set_string("Fill rectangle (solid)","Select upper left corner");
							break;
						case 100: // switch view
							cur_viewing_mode = 1 - cur_viewing_mode;
							need_redraw = TRUE;
							break;
						case 101: 
							set_string("Erase space","Select space to clear");
							overall_mode = 37;
							set_cursor(4);
							break;
						case 102: 
							set_string("Edit sign","Select sign to edit");
							set_cursor(7);
							overall_mode = 38;
							break;
						case 103: 
							overall_mode = 3;
							mode_count = 2;
							set_cursor(5);
							set_string("Create room rectangle","Select upper left corner");
							break;				
						case 104: 
							overall_mode = 2;
							mode_count = 4;
							set_cursor(7);
							set_string("Place first wandering monster arrival point","");
							break;				
						case 105: // replace terrain
							swap_terrain();
							need_redraw = TRUE;
							mouse_button_held = FALSE;
							break;
						// case 106: blank	
						case 107:
							if (editing_town == TRUE) {
								set_string("Can only set town entrances outdoors","");
								break;
								}
							set_string("Set town entrance","Select town to edit");
							set_cursor(7);
							overall_mode = 45;
							break;
							
						case 200:
							if (editing_town == FALSE) {
								set_string("Edit placed item","Not while outdoors.");
								break;
								}
							set_string("Edit placed item","Select item to edit");
							set_cursor(7);
							overall_mode = 5;
							break;
						case 201:
							if (editing_town == FALSE) {
								set_string("Place same item","Not while outdoors.");
								break;
								}
							set_string("Place same item","Select location");
							set_cursor(7);
							overall_mode = 46;
							break;
						case 202:
							if (editing_town == FALSE) {
								set_string("Delete an item","Not while outdoors.");
								break;
								}
							set_string("Delete an item","Select item");
							set_cursor(7);
							overall_mode = 49;
							break;
						case 203:
							set_string("Create/Edit special","Select special location");
							set_cursor(7);
							overall_mode = 40;
							break;
						case 204:
							set_string("Copy special","Select special to copy");
							set_cursor(7);
							overall_mode = 41;
							break;
						case 205:
							if (special_to_paste < 0) {
								set_string("Can't paste special","No special to paste");
								}
							set_string("Paste special","Select location to paste");
							overall_mode = 42;
							set_cursor(7);
							break;
						case 206:
							set_string("Erase special","Select special to erase");
							overall_mode = 43;
							set_cursor(4);
							break;
						case 207:
							set_string("Set/place special","Select special location");
							set_cursor(7);
							overall_mode = 44;
							break;
						
						case 300: 
							set_string("Edit creature","Select creature to edit");
							set_cursor(7);
							overall_mode = 39;
							break;
						case 301: 
							set_string("Place same creature","Select creature location");
							set_cursor(7);
							overall_mode = 8;
							break;
						case 302:
							set_string("Delete a creature","Select creature");
							set_cursor(4);
							overall_mode = 48;
							break;
//						case 303: blank							
						case 304: 
							set_string("Place north entrace","Select entrance location");
							set_cursor(7);
							overall_mode = 10;
							break;
						case 305: 
							set_string("Place west entrace","Select entrance location");
							set_cursor(7);
							overall_mode = 13;
							break;
						case 306: 
							set_string("Place south entrace","Select entrance location");
							set_cursor(7);
							overall_mode = 12;
							break;
						case 307: 
							set_string("Place east entrace","Select entrance location");
							set_cursor(7);
							overall_mode = 11;
							break;
						case 400: 
							set_string("Place web","Select location");
							overall_mode = 20;
							set_cursor(0);
							break;
						case 401: 
							set_string("Place crate","Select location");
							overall_mode = 21;
							set_cursor(0);
							break;
						case 402: 
							set_string("Place barrel","Select location");
							overall_mode = 22;
							set_cursor(0);
							break;
						case 403: 
							set_string("Place fire barrier","Select location");
							overall_mode = 23;
							set_cursor(0);
							break;
						case 404: 
							set_string("Place force barrier","Select location");
							overall_mode = 24;
							set_cursor(0);
							break;
						case 405: 
							set_string("Place quickfire","Select location");
							overall_mode = 25;
							set_cursor(0);
							break;
//						case 406:	blank
						case 407: 
							set_string("Clear space","Select space to clear");
							overall_mode = 26;
							set_cursor(4);
							break;
						case 500: 
							set_string("Place small blood stain","Select stain location");
							overall_mode = 33; mode_count = 0;
							set_cursor(0);
							break;
						case 501: 
							set_string("Place ave. blood stain","Select stain location");
							overall_mode = 33; mode_count = 1;
							set_cursor(0);
							break;
						case 502: 
							set_string("Place large blood stain","Select stain location");
							overall_mode = 33; mode_count = 2;
							set_cursor(0);
							break;
						case 503: 
							set_string("Place small slime pool","Select slime location");
							overall_mode = 33; mode_count = 3;
							set_cursor(0);
							break;
						case 504: 
							set_string("Place large slime pool","Select slime location");
							overall_mode = 33; mode_count = 4;
							set_cursor(0);
							break;
						case 505: 
							set_string("Place ash","Select ash location");
							overall_mode = 33; mode_count = 5;
							set_cursor(0);
							break;
						case 506: 
							set_string("Place bones","Select bones location");
							overall_mode = 33; mode_count = 6;
							set_cursor(0);
							break;
						case 507: 
							set_string("Place rocks","Select rocks location");
							overall_mode = 33; mode_count = 7;					
							set_cursor(0);
							break;
						}
					}
				}
		}
	if (need_redraw == TRUE) {
		draw_main_screen();
		draw_terrain();
		}
		
	return are_done;
}

void flash_rect(RECT to_flash)
{
	int i;
	long dummy;
	HDC hdc;

	hdc = GetDC(mainPtr);
	SetViewportOrgEx(hdc,ulx,uly,NULL);
	InvertRect (hdc,&to_flash);
	play_sound(37);
	Delay(5,&dummy);
	InvertRect (hdc,&to_flash);
	fry_dc(mainPtr,hdc);
}

void swap_terrain()
{
	short a,b,c,i,j;
	unsigned char ter;
	
	change_ter(&a,&b,&c);
	if (a < 0) return;
	
	for (i = 0; i < ((editing_town == TRUE) ? max_dim[town_type] : 48); i++)
		for (j = 0; j < ((editing_town == TRUE) ? max_dim[town_type] : 48); j++) 
		{
			ter = (editing_town == TRUE) ? t_d.terrain[i][j] : current_terrain.terrain[i][j];
			if ((ter == a) && (get_ran(1,1,100) <= c)) {
				if (editing_town == TRUE)
					t_d.terrain[i][j] = b;
					else current_terrain.terrain[i][j] = b;
				}
			}

}

void set_new_terrain(unsigned char selected_terrain)
{
	current_terrain_type = selected_terrain;
	redraw_selected_ter();
	if (selected_terrain < 2)
		current_ground = 0;
		else if (selected_terrain < 5)
			current_ground = 2;
}

Boolean control_key_down()
{
	// is control key down?
	Boolean control_down = FALSE;
	BYTE cBuf[256];
	GetKeyboardState(cBuf);
	if (cBuf[VK_CONTROL] & 0x80)
		control_down = TRUE;
	return control_down;
	
}

Boolean handle_syskeystroke(WPARAM wParam, LPARAM lParam,short *handled)
{
	UINT kp[10] = {VK_NUMPAD0,VK_NUMPAD1, VK_NUMPAD2, VK_NUMPAD3, VK_NUMPAD4,
		VK_NUMPAD5, VK_NUMPAD6, VK_NUMPAD7, VK_NUMPAD8, VK_NUMPAD9};

	POINT terrain_click[10] = {{150,185},{7,359},{50,359},{270,359},
							{7,50},{150,206},{270,50},
								{7,27},{50,27},{275,27}};
	POINT pass_point;
	Boolean are_done = FALSE;
	short i;	
	if (overall_mode >= 60)
		return are_done;

	for (i = 0; i < 10; i++)
		if (wParam == kp[i]) {
				if (i == 0) {
					*handled = 1;
					return handle_keystroke((UINT) 'z',-1);
					}
					else {
					pass_point.x = terrain_click[i].x;
					pass_point.y = terrain_click[i].y;
					wParam = 0;
					FlushEvents(0);
					if (GetKeyState(VK_CONTROL) < 0)  {
						wParam = wParam | MK_CONTROL;
                  }
					are_done = handle_action(pass_point,wParam,-1);
					mouse_button_held = FALSE;
					return are_done;

					}
				}
	return are_done;
}

Boolean handle_keystroke(WPARAM wParam, LPARAM lParam)
{
	Boolean are_done = FALSE;
	POINT pass_point;
	short i,j,k,l,dum;
	long i2,j2,k2,l2,dum2;
	long dummy;
	short the_type,store_ter;
	DWORD s1;
	UINT s2,s3;
	bool ctrl_key = (GetKeyState( VK_CONTROL ) & 0x8000) != 0;	// check MSB for current key state
	Boolean dialog_grabbed_key = FALSE;
	char chr;

	store_ter = current_terrain_type;
	chr = (char) wParam;
		
	switch(chr)
		{
	case'A':
	pass_point.x = RIGHT_AREA_UL_X + 6 + palette_buttons[0][0].left;
	pass_point.y = RIGHT_AREA_UL_Y + 6 + terrain_rects[255].bottom + palette_buttons[0][0].top;
	are_done = handle_action(pass_point,wParam,-1);
	break; 

	case'B':
	pass_point.x = RIGHT_AREA_UL_X + 6 + palette_buttons[1][0].left;
	pass_point.y = RIGHT_AREA_UL_Y + 6 + terrain_rects[255].bottom + palette_buttons[1][0].top;
	are_done = handle_action(pass_point,wParam,-1);
	break; 

	case'C':
	pass_point.x = RIGHT_AREA_UL_X + 6 + palette_buttons[2][0].left;
	pass_point.y = RIGHT_AREA_UL_Y + 6 + terrain_rects[255].bottom + palette_buttons[2][0].top;
	are_done = handle_action(pass_point,wParam,-1);
	break; 

	case'D':
	pass_point.x = RIGHT_AREA_UL_X + 6 + palette_buttons[3][0].left;
	pass_point.y = RIGHT_AREA_UL_Y + 6 + terrain_rects[255].bottom + palette_buttons[3][0].top;
	are_done = handle_action(pass_point,wParam,-1);
	break; 

	case'E':
	pass_point.x = RIGHT_AREA_UL_X + 6 + palette_buttons[4][0].left;
	pass_point.y = RIGHT_AREA_UL_Y + 6 + terrain_rects[255].bottom + palette_buttons[4][0].top;
	are_done = handle_action(pass_point,wParam,-1);
	break; 

	case'F':
	pass_point.x = RIGHT_AREA_UL_X + 6 + palette_buttons[5][0].left;
	pass_point.y = RIGHT_AREA_UL_Y + 6 + terrain_rects[255].bottom + palette_buttons[5][0].top;
	are_done = handle_action(pass_point,wParam,-1);
	break; 

	case'G':
	pass_point.x = RIGHT_AREA_UL_X + 6 + palette_buttons[6][0].left;
	pass_point.y = RIGHT_AREA_UL_Y + 6 + terrain_rects[255].bottom + palette_buttons[6][0].top;
	are_done = handle_action(pass_point,wParam,-1);
	break; 

	case'H':
	pass_point.x = RIGHT_AREA_UL_X + 6 + palette_buttons[7][0].left;
	pass_point.y = RIGHT_AREA_UL_Y + 6 + terrain_rects[255].bottom + palette_buttons[7][0].top;
	are_done = handle_action(pass_point,wParam,-1);
	break; 

	case'I':
	pass_point.x = RIGHT_AREA_UL_X + 6 + palette_buttons[0][1].left;
	pass_point.y = RIGHT_AREA_UL_Y + 6 + terrain_rects[255].bottom + palette_buttons[0][1].top;
	are_done = handle_action(pass_point,wParam,-1);
	break; 

	case'J':
	pass_point.x = RIGHT_AREA_UL_X + 6 + palette_buttons[1][1].left;
	pass_point.y = RIGHT_AREA_UL_Y + 6 + terrain_rects[255].bottom + palette_buttons[1][1].top;
	are_done = handle_action(pass_point,wParam,-1);
	break; 

	case'K':
	pass_point.x = RIGHT_AREA_UL_X + 6 + palette_buttons[2][1].left;
	pass_point.y = RIGHT_AREA_UL_Y + 6 + terrain_rects[255].bottom + palette_buttons[2][1].top;
	are_done = handle_action(pass_point,wParam,-1);
	break; 

	case'L':
	pass_point.x = RIGHT_AREA_UL_X + 6 + palette_buttons[3][1].left;
	pass_point.y = RIGHT_AREA_UL_Y + 6 + terrain_rects[255].bottom + palette_buttons[3][1].top;
	are_done = handle_action(pass_point,wParam,-1);
	break; 

	case'M':
	pass_point.x = RIGHT_AREA_UL_X + 6 + palette_buttons[4][1].left;
	pass_point.y = RIGHT_AREA_UL_Y + 6 + terrain_rects[255].bottom + palette_buttons[4][1].top;
	are_done = handle_action(pass_point,wParam,-1);
	break; 

	case'N':
	pass_point.x = RIGHT_AREA_UL_X + 6 + palette_buttons[5][1].left;
	pass_point.y = RIGHT_AREA_UL_Y + 6 + terrain_rects[255].bottom + palette_buttons[5][1].top;
	are_done = handle_action(pass_point,wParam,-1);
	break; 

	case'O':
	pass_point.x = RIGHT_AREA_UL_X + 6 + palette_buttons[6][1].left;
	pass_point.y = RIGHT_AREA_UL_Y + 6 + terrain_rects[255].bottom + palette_buttons[6][1].top;
	are_done = handle_action(pass_point,wParam,-1);
	break; 

	case'P':
	pass_point.x = RIGHT_AREA_UL_X + 6 + palette_buttons[7][1].left;
	pass_point.y = RIGHT_AREA_UL_Y + 6 + terrain_rects[255].bottom + palette_buttons[7][1].top;
	are_done = handle_action(pass_point,wParam,-1);
	break; 

	case'Q':
	pass_point.x = RIGHT_AREA_UL_X + 6 + palette_buttons[0][2].left;
	pass_point.y = RIGHT_AREA_UL_Y + 6 + terrain_rects[255].bottom + palette_buttons[0][2].top;
	are_done = handle_action(pass_point,wParam,-1);
	break; 

	case'R':
	pass_point.x = RIGHT_AREA_UL_X + 6 + palette_buttons[1][2].left;
	pass_point.y = RIGHT_AREA_UL_Y + 6 + terrain_rects[255].bottom + palette_buttons[1][2].top;
	are_done = handle_action(pass_point,wParam,-1);
	break; 

	case'S':
	pass_point.x = RIGHT_AREA_UL_X + 6 + palette_buttons[2][2].left;
	pass_point.y = RIGHT_AREA_UL_Y + 6 + terrain_rects[255].bottom + palette_buttons[2][2].top;
	are_done = handle_action(pass_point,wParam,-1);
	break; 

	case'T':
	pass_point.x = RIGHT_AREA_UL_X + 6 + palette_buttons[3][2].left;
	pass_point.y = RIGHT_AREA_UL_Y + 6 + terrain_rects[255].bottom + palette_buttons[3][2].top;
	are_done = handle_action(pass_point,wParam,-1);
	break; 

	case'U':
	pass_point.x = RIGHT_AREA_UL_X + 6 + palette_buttons[4][2].left;
	pass_point.y = RIGHT_AREA_UL_Y + 6 + terrain_rects[255].bottom + palette_buttons[4][2].top;
	are_done = handle_action(pass_point,wParam,-1);
	break; 

	case'V':
	pass_point.x = RIGHT_AREA_UL_X + 6 + palette_buttons[5][2].left;
	pass_point.y = RIGHT_AREA_UL_Y + 6 + terrain_rects[255].bottom + palette_buttons[5][2].top;
	are_done = handle_action(pass_point,wParam,-1);
	break; 

	case'W':
	pass_point.x = RIGHT_AREA_UL_X + 6 + palette_buttons[6][2].left;
	pass_point.y = RIGHT_AREA_UL_Y + 6 + terrain_rects[255].bottom + palette_buttons[6][2].top;
	are_done = handle_action(pass_point,wParam,-1);
	break; 

	case'X':
	pass_point.x = RIGHT_AREA_UL_X + 6 + palette_buttons[7][2].left;
	pass_point.y = RIGHT_AREA_UL_Y + 6 + terrain_rects[255].bottom + palette_buttons[7][2].top;
	are_done = handle_action(pass_point,wParam,-1);
	break; 


			case 'Y':
				for (i = 0; i < 64; i++) {
					if ((town.preset_items[i].item_loc.x < 0) ||
						(town.preset_items[i].item_loc.y < 0))
							town.preset_items[i].item_code = -1;		
					if (town.preset_items[i].item_code >= 0) {			
						edit_placed_item(i);
						//town.preset_items[i] = edit_item(town.preset_items[i]);
						}			
					}
				break;

			case '1': case '2': case '3': case '4': case '5': case '6':
				pass_point.x = RIGHT_AREA_UL_X + 6 + palette_buttons[chr - 49][4].left;
				pass_point.y = RIGHT_AREA_UL_Y + 6 + terrain_rects[255].bottom + palette_buttons[chr - 48][4].top;
				are_done = handle_action(pass_point,wParam,-1);
				break;
			case '8':
				pass_point.x = RIGHT_AREA_UL_X + 6 + palette_buttons[7][4].left;
				pass_point.y = RIGHT_AREA_UL_Y + 6 + terrain_rects[255].bottom + palette_buttons[7][4].top;
				are_done = handle_action(pass_point,wParam,-1);
				break; 
			case ',':
							set_string("Edit creature","Select creature to edit");
							overall_mode = 39;
			break;
			case '.':
				set_string("Place same creature","Select creature location");
				overall_mode = 8;
				break;
			case'/':
				pass_point.x = RIGHT_AREA_UL_X + 6 + palette_buttons[2][3].left;
				pass_point.y = RIGHT_AREA_UL_Y + 6 + terrain_rects[255].bottom + palette_buttons[2][3].top;
				are_done = handle_action(pass_point,wParam,-1);
				break; 
			
	case'!':
	pass_point.x = RIGHT_AREA_UL_X + 6 + palette_buttons[0][5].left;
	pass_point.y = RIGHT_AREA_UL_Y + 6 + terrain_rects[255].bottom + palette_buttons[0][5].top;
	are_done = handle_action(pass_point,wParam,-1);
	break; 

	case'@':
	pass_point.x = RIGHT_AREA_UL_X + 6 + palette_buttons[1][5].left;
	pass_point.y = RIGHT_AREA_UL_Y + 6 + terrain_rects[255].bottom + palette_buttons[1][5].top;
	are_done = handle_action(pass_point,wParam,-1);
	break; 

	case'#':
	pass_point.x = RIGHT_AREA_UL_X + 6 + palette_buttons[2][5].left;
	pass_point.y = RIGHT_AREA_UL_Y + 6 + terrain_rects[255].bottom + palette_buttons[2][5].top;
	are_done = handle_action(pass_point,wParam,-1);
	break; 

	case'$':
	pass_point.x = RIGHT_AREA_UL_X + 6 + palette_buttons[3][5].left;
	pass_point.y = RIGHT_AREA_UL_Y + 6 + terrain_rects[255].bottom + palette_buttons[3][5].top;
	are_done = handle_action(pass_point,wParam,-1);
	break; 

	case'%':
	pass_point.x = RIGHT_AREA_UL_X + 6 + palette_buttons[4][5].left;
	pass_point.y = RIGHT_AREA_UL_Y + 6 + terrain_rects[255].bottom + palette_buttons[4][5].top;
	are_done = handle_action(pass_point,wParam,-1);
	break; 

	case'^':
	pass_point.x = RIGHT_AREA_UL_X + 6 + palette_buttons[5][5].left;
	pass_point.y = RIGHT_AREA_UL_Y + 6 + terrain_rects[255].bottom + palette_buttons[5][5].top;
	are_done = handle_action(pass_point,wParam,-1);
	break; 

	case'&':
	pass_point.x = RIGHT_AREA_UL_X + 6 + palette_buttons[6][5].left;
	pass_point.y = RIGHT_AREA_UL_Y + 6 + terrain_rects[255].bottom + palette_buttons[6][5].top;
	are_done = handle_action(pass_point,wParam,-1);
	break; 

	case'*':
	pass_point.x = RIGHT_AREA_UL_X + 6 + palette_buttons[7][5].left;
	pass_point.y = RIGHT_AREA_UL_Y + 6 + terrain_rects[255].bottom + palette_buttons[7][5].top;
	are_done = handle_action(pass_point,wParam,-1);
	break; 

			default:
				Boolean ctrl = control_key_down();
				if ((chr == 4) && (ctrl)) {
					if (editing_town)
							 edit_town_details();
					else outdoor_details();
					break;
				}
							
				if ((chr >= 97) && (chr <= 122)) {
					for (i = 0; i < 256; i++) {
						j = current_terrain_type + i + 1;
						j = j % 256;
						if (scenario.ter_types[j].shortcut_key == chr) {
							set_new_terrain(j);
							i = 256;
							}
						}	
					
					}
				break;
			
			}
	if (store_ter != current_terrain_type)
		draw_terrain();
	mouse_button_held = FALSE;
}


void modify_lists()
{
	set_up_lights();
}
	
void set_up_lights()
{

	short i,j,rad;
	location where,l;
location light_locs[40];
short num_lights = 0;
Boolean where_lit[64][64];

		
	// Find bonfires, braziers, etc.
	num_lights = 0;
	for (i = 0; i < 64; i++)
		for (j = 0; j < 64; j++)
			where_lit[i][j] = 0;
			
	for (i = 0; i < max_dim[town_type]; i++)
		for (j = 0; j < max_dim[town_type]; j++) {
			l.x = i; l.y = j;
			rad = scenario.ter_types[t_d.terrain[i][j]].light_radius;
			if (rad > 0) {
				for (where.x = max(0,i - rad); where.x < min(max_dim[town_type],i + rad + 1); where.x++)
					for (where.y = max(0,j - rad); where.y < min(max_dim[town_type],j + rad + 1); where.y++)
						if ((where_lit[where.x][where.y] == 0) && (dist(where,l) <= rad) && (can_see(l,where,0) < 5))
							where_lit[where.x][where.y] = 1;
				}
			}
	for (i = 0; i < 8; i++)	
		for (j = 0; j < 64; j++)
			t_d.lighting[i][j] = 0;
	for (where.x = 0; where.x < max_dim[town_type]; where.x++)
		for (where.y = 0; where.y < max_dim[town_type]; where.y++) {
			if (where_lit[where.x][where.y] > 0) {
				t_d.lighting[where.x / 8][where.y] = t_d.lighting[where.x / 8][where.y] | (char) (s_pow(2,where.x % 8));
				}
		}	

}

	


Boolean is_wall(short i,short j)
{

	unsigned char ter,walls[3] = {77,77,224};
	Boolean answer = FALSE;
	short k,pic;
	
	ter = (editing_town == TRUE) ? t_d.terrain[i][j] : current_terrain.terrain[i][j];
	pic = scenario.ter_types[ter].picture;
	
	if ((editing_town == TRUE) && ((i < 0) || (i > max_dim[town_type] - 1) || (j < 0) || (j > max_dim[town_type] - 1)))
		return TRUE;
	if ((editing_town == FALSE) && ((i < 0) || (i > 47) || (j < 0) || (j > 47)))
		return TRUE;
	if ((ter < 22) && (ter > 4))
		return TRUE;
	if ((pic >= 5) && (pic <= 17))
		return TRUE;
	if ((pic >= 88) && (pic <= 120))
		return TRUE;
	if ((pic >= 240) && (pic <= 243))
		return TRUE;
	if (pic == 405)
		return TRUE;
//	for (k = 0; k < 3 ; k++)
//		if (t_d.terrain[i][j] == walls[k]) 
//			answer = TRUE;	
	return answer;		

}

Boolean is_correctable_wall(short i,short j)
{

	unsigned char ter,walls[13] = {5,6,8,9,11,12,
					14,15,17,18,19,20,21};
	Boolean answer = FALSE;
	short k;

	ter = (editing_town == TRUE) ? t_d.terrain[i][j] : current_terrain.terrain[i][j];
	if ((editing_town == TRUE) && ((i < 0) || (i > max_dim[town_type] - 1) || (j < 0) || (j > max_dim[town_type] - 1)))
		return FALSE;
	if ((editing_town == FALSE) && ((i < 0) || (i > 47) || (j < 0) || (j > 47)))
		return FALSE;
	for (k = 0; k < 13 ; k++)
		if (ter == walls[k]) 
			answer = TRUE;	
	return answer;		

}

Boolean is_mountain(short i,short j)
{

	Boolean answer = FALSE;
	short k,pic;
	unsigned char ter;
	
	ter = (editing_town == TRUE) ? t_d.terrain[i][j] : current_terrain.terrain[i][j];
	pic = scenario.ter_types[ter].picture;
	if ((editing_town == TRUE) && ((i < 0) || (i > max_dim[town_type] - 1) || (j < 0) || (j > max_dim[town_type] - 1)))
		return TRUE;
	if ((editing_town == FALSE) && ((i < 0) || (i > 47) || (j < 0) || (j > 47)))
		return TRUE;
	if ((ter >= 22) && (ter <= 35))
		answer = TRUE;
		if ((pic >= 18) &&
			(pic <= 31) ) return TRUE;
	if (pic == 192) return TRUE;
	if (pic == 193) return TRUE;
	if (pic == 195) return TRUE;
	if (pic == 194) return TRUE;
	return answer;		

}
Boolean is_hill(short i,short j)
{

	Boolean answer = FALSE;
	short k,pic;
	unsigned char ter;
	
	ter = (editing_town == TRUE) ? t_d.terrain[i][j] : current_terrain.terrain[i][j];
	pic = scenario.ter_types[ter].picture;
	if ((editing_town == TRUE) && ((i < 0) || (i > max_dim[town_type] - 1) || (j < 0) || (j > max_dim[town_type] - 1)))
		return TRUE;
	if ((editing_town == FALSE) && ((i < 0) || (i > 47) || (j < 0) || (j > 47)))
		return TRUE;
	if ((ter >= 36) && (ter <= 49))
		answer = TRUE;
  		if (((pic >= 32) &&
			(pic <= 45)) 
			|| (pic == 204)
			|| (pic == 212)
			)
			answer = TRUE;	
	return answer;		

}
Boolean is_hill_or_mountain(short i,short j)
{
	if (is_hill(i,j))
		return TRUE;
	if (is_mountain(i,j))
		return TRUE;
	return FALSE;
}
Boolean is_erasable_water(short i,short j)
{

	Boolean answer = FALSE;
	short k,pic;
	unsigned char ter;
	
	ter = (editing_town == TRUE) ? t_d.terrain[i][j] : current_terrain.terrain[i][j];
	pic = scenario.ter_types[ter].picture;
	if ((editing_town == TRUE) && ((i < 0) || (i > max_dim[town_type] - 1) || (j < 0) || (j > max_dim[town_type] - 1)))
		return FALSE;
	if ((editing_town == FALSE) && ((i < 0) || (i > 47) || (j < 0) || (j > 47)))
		return FALSE;
		if ((ter >= 50) &&
			(ter <= 62))
			answer = TRUE;	

	return answer;		
}
Boolean is_water(short i,short j)
{

	Boolean answer = FALSE;
	short k,pic;
	unsigned char ter;
	
	ter = (editing_town == TRUE) ? t_d.terrain[i][j] : current_terrain.terrain[i][j];
	pic = scenario.ter_types[ter].picture;
	if ((editing_town == TRUE) && ((i < 0) || (i > max_dim[town_type] - 1) || (j < 0) || (j > max_dim[town_type] - 1)))
		return TRUE;
	if ((editing_town == FALSE) && ((i < 0) || (i > 47) || (j < 0) || (j > 47)))
		return TRUE;
		if ((pic >= 46) &&
			(pic <= 66))
			answer = TRUE;	

	return answer;		
}
Boolean is_correctable_water(short i,short j)
{
	Boolean answer = FALSE;
	short k,pic;
	unsigned char ter;
	
	ter = (editing_town == TRUE) ? t_d.terrain[i][j] : current_terrain.terrain[i][j];
	pic = scenario.ter_types[ter].picture;
	if ((editing_town == TRUE) && ((i < 0) || (i > max_dim[town_type] - 1) || (j < 0) || (j > max_dim[town_type] - 1)))
		return FALSE;
	if ((editing_town == FALSE) && ((i < 0) || (i > 47) || (j < 0) || (j > 47)))
		return FALSE;
		if ((ter >= 50) &&
			(ter <= 62))
			answer = TRUE;	

	return answer;		
}

void shy_change_circle_terrain(location center,short radius,unsigned char terrain_type,short probability)
// prob is 0 - 20, 0 no, 20 always
{
location l;
short i,j;
	unsigned char ter;
	
for (i = 0; i < ((editing_town == TRUE) ? max_dim[town_type] : 48); i++)
	for (j = 0; j < ((editing_town == TRUE) ? max_dim[town_type] : 48); j++) {
		l.x = i;
		l.y = j;
		ter = (editing_town == TRUE) ? t_d.terrain[i][j] : current_terrain.terrain[i][j];
		if ((dist(center,l) <= radius) && (get_ran(1,1,20) <= probability) 
			&& (ter < 5))
			set_terrain(l,terrain_type);
		}
}

void change_circle_terrain(location center,short radius,unsigned char terrain_type,short probability)
// prob is 0 - 20, 0 no, 20 always
{
location l;
short i,j;

for (i = 0; i < ((editing_town == TRUE) ? max_dim[town_type] : 48); i++)
	for (j = 0; j < ((editing_town == TRUE) ? max_dim[town_type] : 48); j++) {
		l.x = i;
		l.y = j;
		if ((dist(center,l) <= radius) && (get_ran(1,1,20) <= probability))
			set_terrain(l,terrain_type);
		}
}

void change_rect_terrain(RECT r,unsigned char terrain_type,short probability,Boolean hollow)
// prob is 0 - 20, 0 no, 20 always
{
location l;
short i,j;

for (i = 0; i < ((editing_town == TRUE) ? max_dim[town_type] : 48); i++)
	for (j = 0; j < ((editing_town == TRUE) ? max_dim[town_type] : 48); j++) {
		l.x = i;
		l.y = j;
		if ((i >= r.left) && (i <= r.right) && (j >= r.top) && (j <= r.bottom)
		&& ((hollow == FALSE) || (i == r.left) || (i == r.right) || (j == r.top) || (j == r.bottom))
		  && ((hollow == TRUE) || (get_ran(1,1,20) <= probability)))
			set_terrain(l,terrain_type);
		}
}


void frill_up_terrain()
{
	short i,j;
	unsigned char terrain_type;
	
	for (i = 0; i < ((editing_town == TRUE) ? max_dim[town_type] : 48); i++)
		for (j = 0; j < ((editing_town == TRUE) ? max_dim[town_type] : 48); j++) {
		terrain_type = (editing_town == TRUE) ? t_d.terrain[i][j] : current_terrain.terrain[i][j];

		if ((terrain_type == 2) && (get_ran(1,1,20) < 3))
			terrain_type = 3;
		if ((terrain_type == 2) && (get_ran(1,1,20) < 2))
			terrain_type = 4;
		if ((terrain_type == 0) && (get_ran(1,1,20) < 2))
			terrain_type = 1;
		if ((terrain_type == 36) && (get_ran(1,1,20) < 5))
			terrain_type = 37;
		if (editing_town == TRUE)
			t_d.terrain[i][j] = terrain_type;
			else current_terrain.terrain[i][j] = terrain_type;
		}
	draw_terrain();
}

void unfrill_terrain()
{
	short i,j;
	unsigned char terrain_type;
	
	for (i = 0; i < ((editing_town == TRUE) ? max_dim[town_type] : 48); i++)
		for (j = 0; j < ((editing_town == TRUE) ? max_dim[town_type] : 48); j++) {
		terrain_type = (editing_town == TRUE) ? t_d.terrain[i][j] : current_terrain.terrain[i][j];
		if (terrain_type == 3)
			terrain_type = 2;
		if (terrain_type == 4) 
			terrain_type = 2;
		if (terrain_type == 1)
			terrain_type = 0;
		if (terrain_type == 37)
			terrain_type = 36;
		if (editing_town == TRUE)
			t_d.terrain[i][j] = terrain_type;
			else current_terrain.terrain[i][j] = terrain_type;
		}
	draw_terrain();
}

void set_terrain(location l,unsigned char terrain_type)
{
	short i,j,which_sign = -1;
	unsigned char ter;
	location l2;
	
	i = l.x; j = l.y;
	//if ((l.x < 0) || (l.x > max_dim[town_type] - 1) || (l.y < 0) || (l.y > max_dim[town_type] - 1))
	//	return;
	if ((editing_town == TRUE) && ((i < 0) || (i > max_dim[town_type] - 1) || (j < 0) || (j > max_dim[town_type] - 1)))
		return ;
	if ((editing_town == FALSE) && ((i < 0) || (i > 47) || (j < 0) || (j > 47)))
		return ;

	if (editing_town == TRUE)
		t_d.terrain[i][j] = terrain_type;
		else current_terrain.terrain[i][j] = terrain_type;
	l2 = l;
	
	if (terrain_type == 85) {
		l2.x++;	
		if (editing_town == TRUE)
			t_d.terrain[l2.x][l2.y] = 86;
			else current_terrain.terrain[l2.x][l2.y] = 86;
		}
	if (terrain_type == 86) {
		l2.x--;
		if (editing_town == TRUE)
			t_d.terrain[l2.x][l2.y] = 85;
			else current_terrain.terrain[l2.x][l2.y] = 85;
		}
	if (terrain_type == 88) {
		l2.x++;
		if (editing_town == TRUE)
			t_d.terrain[l2.x][l2.y] = 89;
			else current_terrain.terrain[l2.x][l2.y] = 89;
		}
	if (terrain_type == 89) {
		l2.x--;
		if (editing_town == TRUE)
			t_d.terrain[l2.x][l2.y] = 88;
			else current_terrain.terrain[l2.x][l2.y] = 88;
		}

	if (is_mountain(l.x,l.y))
	 for (i = l.x - 1; i < l.x + 2; i++)
	 	for (j = l.y - 1; j < l.y + 2; j++) {
			if ((i < 0) || (i > ((editing_town == TRUE) ? max_dim[town_type] - 1 : 47)) 
				|| (j < 0) || (j > ((editing_town == TRUE) ? max_dim[town_type] - 1 : 47)))
				;
				else {
					l2.x = i;
					l2.y = j;
					ter = (editing_town == TRUE) ? t_d.terrain[l2.x][l2.y] : current_terrain.terrain[l2.x][l2.y];
					if ((!is_mountain(l2.x,l2.y)) && (ter < 5)) {	
						if (editing_town == TRUE)
							t_d.terrain[l2.x][l2.y] = 36;
							else current_terrain.terrain[l2.x][l2.y] = 36;
						}
					}
	 		}
	 		
	adjust_space(l);
	l.x--;
	adjust_space(l);
	
	l.x += 2;
	adjust_space(l);
	
	l.x--;
	l.y--;
	adjust_space(l);
	
	l.y+= 2;
	adjust_space(l);
	l.y--;
	
	if ((scenario.ter_types[terrain_type].special == 11) && (editing_town == TRUE)) { /// it's a sign
		for (i = 0; i < 15; i++) 
			if (which_sign < 0) {
				if ((town.sign_locs[i].x == l.x) && (town.sign_locs[i].y == l.y))
					which_sign = i;
				}
		for (i = 0; i < 15; i++) 
			if (which_sign < 0) {
			if (town.sign_locs[i].x == 100) 
				which_sign = i;
				else {
					ter = t_d.terrain[town.sign_locs[i].x][town.sign_locs[i].y];
					if (scenario.ter_types[ter].special != 11)
						which_sign = i;
					}
			}
		if (which_sign >= 0) {
			town.sign_locs[which_sign] = l;
			edit_sign(which_sign,scenario.ter_types[terrain_type].picture);
			sign_error_received = FALSE;
			}
			else {
				t_d.terrain[l.x][l.y] = current_ground;
				if (sign_error_received == FALSE) {
					give_error("Towns can have at most 15 signs. Outdoor sections can have at most 8. When the party looks at this sign, they will get no message.","",0);
					sign_error_received = TRUE;
					}
				}
		mouse_button_held = FALSE;
		}
	if ((scenario.ter_types[terrain_type].special == 11) && (editing_town == FALSE)) { /// it's a sign
		if ((l.x == 0) || (l.x == 47) || (l.y == 0) || (l.y == 47)) {
			fancy_choice_dialog(870,0);
			mouse_button_held = FALSE;
			return;
			}
		for (i = 0; i < 8; i++) 
			if (which_sign < 0) {
				if ((current_terrain.sign_locs[i].x == l.x) && (current_terrain.sign_locs[i].y == l.y))
					which_sign = i;
				}
		for (i = 0; i < 8; i++) 
			if (which_sign < 0) {
			if (current_terrain.sign_locs[i].x == 100) 
				which_sign = i;
				else {
					ter = current_terrain.terrain[current_terrain.sign_locs[i].x][current_terrain.sign_locs[i].y];
					if (scenario.ter_types[ter].special != 11)
						which_sign = i;
					}
			}
		if (which_sign >= 0) {
			current_terrain.sign_locs[which_sign] = l;
			edit_sign(which_sign,scenario.ter_types[terrain_type].picture);
			sign_error_received = FALSE;
			}
			else {
				current_terrain.terrain[l.x][l.y] = current_ground;
				if (sign_error_received == FALSE) {
					give_error("Towns can have at most 15 signs. Outdoor sections can have at most 8. When the party looks at this sign, they will get no message.","",0);
					sign_error_received = TRUE;
					}
				}
		mouse_button_held = FALSE;
		}
}

Boolean fix_rubble(location l)
{
	unsigned char terrain_type;
	
	if ((l.x < 0) || (l.y > max_dim[town_type] - 1) || (l.y < 0) || (l.y > max_dim[town_type] - 1))
		return FALSE;
	terrain_type = t_d.terrain[l.x][l.y];
	if ((terrain_type == 85) && (t_d.terrain[l.x + 1][l.y] != 86)) {
		t_d.terrain[l.x][l.y] = 0;
		return TRUE;
		}
	if ((terrain_type == 86) && (t_d.terrain[l.x - 1][l.y] != 85)) {
		t_d.terrain[l.x][l.y] = 0;
		return TRUE;
		}
	if ((terrain_type == 88) && (t_d.terrain[l.x + 1][l.y] != 89)) {
		t_d.terrain[l.x][l.y] = 2;
		return TRUE;
		}
	if ((terrain_type == 89) && (t_d.terrain[l.x - 1][l.y] != 88)) {
		t_d.terrain[l.x][l.y] = 2;
		return TRUE;
		}
	return FALSE;
}

Boolean fix_cave(location l)
{
	unsigned char ter_to_fix;
	unsigned char store_ter;
	short i,j;
	
	if ((l.x < 0) || (l.y > max_dim[town_type] - 1) || (l.y < 0) || (l.y > max_dim[town_type] - 1))
		return FALSE;
	i = l.x; j = l.y;
	store_ter = t_d.terrain[l.x][l.y];
	
	if (is_correctable_wall(i,j) == TRUE) {
/*	if (((l.x == 0) || (l.x == max_dim[town_type] - 1)) && ((l.y == 0) || (l.y == max_dim[town_type] - 1))) 
		ter_to_fix = 2;
	else if ((l.x == 0) || (l.x == max_dim[town_type] - 1)) {
		if (is_wall(i,j-1)) {
			if (is_wall(i,j+1)) {
				ter_to_fix = 2;
				}
				else ter_to_fix = 3;
			}
			else ter_to_fix = 9;
		}
	else if ((l.y == 0) || (l.y == max_dim[town_type] - 1)) {
		if (is_wall(i - 1,j)) {
			if (is_wall(i + 1,j)) {
				ter_to_fix = 2;
				}
				else ter_to_fix = 6;
			}
			else ter_to_fix = 12;
	
		}
	else { // not edge
*/
if (is_wall(i-1,j) == FALSE)
{
	if (is_wall(i,j-1) == FALSE)
	{
		ter_to_fix = 11;
	}
	else
	{
		if (is_wall(i,j+1) == FALSE)
			ter_to_fix = 14;
			else ter_to_fix = 12;
	}



}					

else // wall(i-1,j) == TRUE

{
	if (is_wall(i+1,j) == FALSE)
	{
		if (is_wall(i, j-1) == FALSE)
			ter_to_fix = 8;
			else if (is_wall(i,j+1) == FALSE)
				ter_to_fix = 5;
				else ter_to_fix = 6;
	
	}
	
	else //wall(i+1,j) == TRUE
	
	{
	if (is_wall(i, j-1) == FALSE)
		ter_to_fix = 9;
		else if (is_wall(i,j+1) == FALSE)
			ter_to_fix = 3;
			else if (is_wall(i-1,j-1) == FALSE)
				ter_to_fix = 16;
				else if(is_wall(i-1,j+1) == FALSE)
					ter_to_fix = 15;
					else if (is_wall(i+1,j-1) == FALSE)
						ter_to_fix = 17;
						else if (is_wall(i+1,j+1) == FALSE)
						ter_to_fix = 18;
						else ter_to_fix = 2; 
	
	
	
	}

}
//}
			ter_to_fix += 3;
			t_d.terrain[i][j] = ter_to_fix; 				
			}

	if (store_ter == t_d.terrain[l.x][l.y])
		return FALSE;
		else return TRUE;

}


Boolean fix_mountain(location l)
{
	unsigned char ter_to_fix;
	unsigned char store_ter;
	short i,j;
	
	if ((l.x < 0) || (l.y > max_dim[town_type] - 1) || (l.y < 0) || (l.y > max_dim[town_type] - 1))
		return FALSE;
	i = l.x; j = l.y;
	store_ter = t_d.terrain[l.x][l.y];
	
	if ((store_ter >= 22) &&
			(store_ter <= 35) && (store_ter != 23)) {
	if (((l.x == 0) || (l.x == max_dim[town_type] - 1)) && ((l.y == 0) || (l.y == max_dim[town_type] - 1))) 
		ter_to_fix = 22;
	else if ((l.x == 0) || (l.x == max_dim[town_type] - 1)) {
		if (is_mountain(i,j-1)) {
			if (is_mountain(i,j+1)) {
				ter_to_fix = 22;
				}
				else ter_to_fix = 24;
			}
			else ter_to_fix = 28;
		}
	else if ((l.y == 0) || (l.y == max_dim[town_type] - 1)) {
		if (is_mountain(i - 1,j)) {
			if (is_mountain(i + 1,j)) {
				ter_to_fix = 22;
				}
				else ter_to_fix = 26;
			}
			else ter_to_fix = 30;
	
		}
	else { // not edge

if (is_mountain(i-1,j) == FALSE)
{
	if (is_mountain(i,j-1) == FALSE)
	{
		ter_to_fix = 29;
	}
	else
	{
		if (is_mountain(i,j+1) == FALSE)
			ter_to_fix = 31;
			else ter_to_fix = 30;
	}



}					

else // wall(i-1,j) == TRUE

{
	if (is_mountain(i+1,j) == FALSE)
	{
		if (is_mountain(i, j-1) == FALSE)
			ter_to_fix = 27;
			else if (is_mountain(i,j+1) == FALSE)
				ter_to_fix = 25;
				else ter_to_fix = 26;
	
	}
	
	else //wall(i+1,j) == TRUE
	
	{
	if (is_mountain(i, j-1) == FALSE)
		ter_to_fix = 28;
		else if (is_mountain(i,j+1) == FALSE)
			ter_to_fix = 24;
			else if (is_mountain(i-1,j-1) == FALSE)
				ter_to_fix = 33;
				else if(is_mountain(i-1,j+1) == FALSE)
					ter_to_fix = 32;
					else if (is_mountain(i+1,j-1) == FALSE)
						ter_to_fix = 34;
						else if (is_mountain(i+1,j+1) == FALSE)
						ter_to_fix = 35;
						else ter_to_fix = 22; 
	
	
	
	}

}
}
			t_d.terrain[i][j] = ter_to_fix; 				
			}

	if (store_ter == t_d.terrain[l.x][l.y])
		return FALSE;
		else return TRUE;

}

Boolean fix_hill(location l)
{
	unsigned char ter_to_fix;
	unsigned char store_ter;
	short i,j;
	
	if ((l.x < 0) || (l.y > max_dim[town_type] - 1) || (l.y < 0) || (l.y > max_dim[town_type] - 1))
		return FALSE;
	i = l.x; j = l.y;
	store_ter = t_d.terrain[l.x][l.y];
	
	if ((t_d.terrain[l.x][l.y] >= 36) &&
			(t_d.terrain[l.x][l.y] <= 49) ) {
	if (((l.x == 0) || (l.x == max_dim[town_type] - 1)) && ((l.y == 0) || (l.y == max_dim[town_type] - 1))) 
		ter_to_fix = 22;
	else if ((l.x == 0) || (l.x == max_dim[town_type] - 1)) {
		if (is_hill_or_mountain(i,j-1)) {
			if (is_hill_or_mountain(i,j+1)) {
				ter_to_fix = 22;
				}
				else ter_to_fix = 24;
			}
			else ter_to_fix = 28;
		}
	else if ((l.y == 0) || (l.y == max_dim[town_type] - 1)) {
		if (is_hill_or_mountain(i - 1,j)) {
			if (is_hill_or_mountain(i + 1,j)) {
				ter_to_fix = 22;
				}
				else ter_to_fix = 26;
			}
			else ter_to_fix = 30;
	
		}
	else { // not edge

if (is_hill_or_mountain(i-1,j) == FALSE)
{
	if (is_hill_or_mountain(i,j-1) == FALSE)
	{
		ter_to_fix = 29;
	}
	else
	{
		if (is_hill_or_mountain(i,j+1) == FALSE)
			ter_to_fix = 31;
			else ter_to_fix = 30;
	}



}					

else // wall(i-1,j) == TRUE

{
	if (is_hill_or_mountain(i+1,j) == FALSE)
	{
		if (is_hill_or_mountain(i, j-1) == FALSE)
			ter_to_fix = 27;
			else if (is_hill_or_mountain(i,j+1) == FALSE)
				ter_to_fix = 25;
				else ter_to_fix = 26;
	
	}
	
	else //wall(i+1,j) == TRUE
	
	{
	if (is_hill_or_mountain(i, j-1) == FALSE)
		ter_to_fix = 28;
		else if (is_hill_or_mountain(i,j+1) == FALSE)
			ter_to_fix = 24;
			else if (is_hill_or_mountain(i-1,j-1) == FALSE)
				ter_to_fix = 33;
				else if(is_hill_or_mountain(i-1,j+1) == FALSE)
					ter_to_fix = 32;
					else if (is_hill_or_mountain(i+1,j-1) == FALSE)
						ter_to_fix = 34;
						else if (is_hill_or_mountain(i+1,j+1) == FALSE)
						ter_to_fix = 35;
						else ter_to_fix = 22; 
	
	
	
	}

}
}
			ter_to_fix += 14;
			//if ((ter_to_fix == 36) && (get_ran(1,0,15) == 5))
			//	ter_to_fix++; 
			if ((t_d.terrain[i][j] != 37) || (ter_to_fix != 36))
				t_d.terrain[i][j] = ter_to_fix; 				
			}

	if (store_ter == t_d.terrain[l.x][l.y])
		return FALSE;
		else return TRUE;

}

Boolean fix_water(location l)
{
	unsigned char ter_to_fix;
	unsigned char store_ter;
	short i,j;
	
	if ((l.x < 0) || (l.y > max_dim[town_type] - 1) || (l.y < 0) || (l.y > max_dim[town_type] - 1))
		return FALSE;
	i = l.x; j = l.y;
	store_ter = t_d.terrain[l.x][l.y];
	
	if (is_correctable_water(l.x,l.y)) {
	if (((l.x == 0) || (l.x == max_dim[town_type] - 1)) && ((l.y == 0) || (l.y == max_dim[town_type] - 1))) 
		ter_to_fix = 22;
	else if ((l.x == 0) || (l.x == max_dim[town_type] - 1)) {
		if (is_water(i,j-1)) {
			if (is_water(i,j+1)) {
				ter_to_fix = 22;
				}
				else ter_to_fix = 24;
			}
			else ter_to_fix = 28;
		}
	else if ((l.y == 0) || (l.y == max_dim[town_type] - 1)) {
		if (is_water(i - 1,j)) {
			if (is_water(i + 1,j)) {
				ter_to_fix = 22;
				}
				else ter_to_fix = 26;
			}
			else ter_to_fix = 30;
	
		}
	else { // not edge

if (is_water(i-1,j) == FALSE)
{
	if (is_water(i,j-1) == FALSE)
	{
		ter_to_fix = 29;
	}
	else
	{
		if (is_water(i,j+1) == FALSE)
			ter_to_fix = 31;
			else ter_to_fix = 30;
	}



}					

else // wall(i-1,j) == TRUE

{
	if (is_water(i+1,j) == FALSE)
	{
		if (is_water(i, j-1) == FALSE)
			ter_to_fix = 27;
			else if (is_water(i,j+1) == FALSE)
				ter_to_fix = 25;
				else ter_to_fix = 26;
	
	}
	
	else //wall(i+1,j) == TRUE
	
	{
	if (is_water(i, j-1) == FALSE)
		ter_to_fix = 28;
		else if (is_water(i,j+1) == FALSE)
			ter_to_fix = 24;
			else if (is_water(i-1,j-1) == FALSE)
				ter_to_fix = 33;
				else if(is_water(i-1,j+1) == FALSE)
					ter_to_fix = 32;
					else if (is_water(i+1,j-1) == FALSE)
						ter_to_fix = 34;
						else if (is_water(i+1,j+1) == FALSE)
						ter_to_fix = 35;
						else ter_to_fix = 22; 
	
	
	
	}

}
}
			ter_to_fix += 27;
			if (ter_to_fix == 49)
				ter_to_fix++;
			t_d.terrain[i][j] = ter_to_fix; 				
			}

	if (store_ter == t_d.terrain[l.x][l.y])
		return FALSE;
		else return TRUE;

}

// begin out
Boolean out_fix_rubble(location l)
{
	unsigned char terrain_type;
	
	if ((l.x < 0) || (l.y > 47) || (l.y < 0) || (l.y > 47))
		return FALSE;
	terrain_type = current_terrain.terrain[l.x][l.y];
	if ((terrain_type == 85) && (current_terrain.terrain[l.x + 1][l.y] != 86)) {
		current_terrain.terrain[l.x][l.y] = 0;
		return TRUE;
		}
	if ((terrain_type == 86) && (current_terrain.terrain[l.x - 1][l.y] != 85)) {
		current_terrain.terrain[l.x][l.y] = 0;
		return TRUE;
		}
	if ((terrain_type == 88) && (current_terrain.terrain[l.x + 1][l.y] != 89)) {
		current_terrain.terrain[l.x][l.y] = 2;
		return TRUE;
		}
	if ((terrain_type == 89) && (current_terrain.terrain[l.x - 1][l.y] != 88)) {
		current_terrain.terrain[l.x][l.y] = 2;
		return TRUE;
		}
	return FALSE;
}

Boolean out_fix_cave(location l)
{
	unsigned char ter_to_fix;
	unsigned char store_ter;
	short i,j;
	
	if ((l.x < 0) || (l.y > 47) || (l.y < 0) || (l.y > 47))
		return FALSE;
	i = l.x; j = l.y;
	store_ter = current_terrain.terrain[l.x][l.y];
	
	if (is_correctable_wall(i,j) == TRUE) {
	if (((l.x == 0) || (l.x == 47)) && ((l.y == 0) || (l.y == 47))) 
		ter_to_fix = 2;
	else if ((l.x == 0) || (l.x == 47)) {
		if (is_wall(i,j-1)) {
			if (is_wall(i,j+1)) {
				ter_to_fix = 2;
				}
				else ter_to_fix = 3;
			}
			else ter_to_fix = 9;
		}
	else if ((l.y == 0) || (l.y == 47)) {
		if (is_wall(i - 1,j)) {
			if (is_wall(i + 1,j)) {
				ter_to_fix = 2;
				}
				else ter_to_fix = 6;
			}
			else ter_to_fix = 12;
	
		}
	else { // not edge

if (is_wall(i-1,j) == FALSE)
{
	if (is_wall(i,j-1) == FALSE)
	{
		ter_to_fix = 11;
	}
	else
	{
		if (is_wall(i,j+1) == FALSE)
			ter_to_fix = 14;
			else ter_to_fix = 12;
	}



}					

else // wall(i-1,j) == TRUE

{
	if (is_wall(i+1,j) == FALSE)
	{
		if (is_wall(i, j-1) == FALSE)
			ter_to_fix = 8;
			else if (is_wall(i,j+1) == FALSE)
				ter_to_fix = 5;
				else ter_to_fix = 6;
	
	}
	
	else //wall(i+1,j) == TRUE
	
	{
	if (is_wall(i, j-1) == FALSE)
		ter_to_fix = 9;
		else if (is_wall(i,j+1) == FALSE)
			ter_to_fix = 3;
			else if (is_wall(i-1,j-1) == FALSE)
				ter_to_fix = 16;
				else if(is_wall(i-1,j+1) == FALSE)
					ter_to_fix = 15;
					else if (is_wall(i+1,j-1) == FALSE)
						ter_to_fix = 17;
						else if (is_wall(i+1,j+1) == FALSE)
						ter_to_fix = 18;
						else ter_to_fix = 2; 
	
	
	
	}

}
}
			ter_to_fix += 3;
			current_terrain.terrain[i][j] = ter_to_fix; 				
			}

	if (store_ter == current_terrain.terrain[l.x][l.y])
		return FALSE;
		else return TRUE;

}


Boolean out_fix_mountain(location l)
{
	unsigned char ter_to_fix;
	unsigned char store_ter;
	short i,j;
	
	if ((l.x < 0) || (l.y > 47) || (l.y < 0) || (l.y > 47))
		return FALSE;
	i = l.x; j = l.y;
	store_ter = current_terrain.terrain[l.x][l.y];
	
	if ((store_ter >= 22) &&
			(store_ter <= 35) && (store_ter != 23)) {
	if (((l.x == 0) || (l.x == 47)) && ((l.y == 0) || (l.y == 47))) 
		ter_to_fix = 22;
	else if ((l.x == 0) || (l.x == 47)) {
		if (is_mountain(i,j-1)) {
			if (is_mountain(i,j+1)) {
				ter_to_fix = 22;
				}
				else ter_to_fix = 24;
			}
			else ter_to_fix = 28;
		}
	else if ((l.y == 0) || (l.y == 47)) {
		if (is_mountain(i - 1,j)) {
			if (is_mountain(i + 1,j)) {
				ter_to_fix = 22;
				}
				else ter_to_fix = 26;
			}
			else ter_to_fix = 30;
	
		}
	else { // not edge

if (is_mountain(i-1,j) == FALSE)
{
	if (is_mountain(i,j-1) == FALSE)
	{
		ter_to_fix = 29;
	}
	else
	{
		if (is_mountain(i,j+1) == FALSE)
			ter_to_fix = 31;
			else ter_to_fix = 30;
	}



}					

else // wall(i-1,j) == TRUE

{
	if (is_mountain(i+1,j) == FALSE)
	{
		if (is_mountain(i, j-1) == FALSE)
			ter_to_fix = 27;
			else if (is_mountain(i,j+1) == FALSE)
				ter_to_fix = 25;
				else ter_to_fix = 26;
	
	}
	
	else //wall(i+1,j) == TRUE
	
	{
	if (is_mountain(i, j-1) == FALSE)
		ter_to_fix = 28;
		else if (is_mountain(i,j+1) == FALSE)
			ter_to_fix = 24;
			else if (is_mountain(i-1,j-1) == FALSE)
				ter_to_fix = 33;
				else if(is_mountain(i-1,j+1) == FALSE)
					ter_to_fix = 32;
					else if (is_mountain(i+1,j-1) == FALSE)
						ter_to_fix = 34;
						else if (is_mountain(i+1,j+1) == FALSE)
						ter_to_fix = 35;
						else ter_to_fix = 22; 
	
	
	
	}

}
}
			current_terrain.terrain[i][j] = ter_to_fix; 				
			}

	if (store_ter == current_terrain.terrain[l.x][l.y])
		return FALSE;
		else return TRUE;

}

Boolean out_fix_hill(location l)
{
	unsigned char ter_to_fix;
	unsigned char store_ter;
	short i,j;
	
	if ((l.x < 0) || (l.y > 47) || (l.y < 0) || (l.y > 47))
		return FALSE;
	i = l.x; j = l.y;
	store_ter = current_terrain.terrain[l.x][l.y];
	
	if ((current_terrain.terrain[l.x][l.y] >= 36) &&
			(current_terrain.terrain[l.x][l.y] <= 49) ) {
	if (((l.x == 0) || (l.x == 47)) && ((l.y == 0) || (l.y == 47))) 
		ter_to_fix = 22;
	else if ((l.x == 0) || (l.x == 47)) {
		if (is_hill_or_mountain(i,j-1)) {
			if (is_hill_or_mountain(i,j+1)) {
				ter_to_fix = 22;
				}
				else ter_to_fix = 24;
			}
			else ter_to_fix = 28;
		}
	else if ((l.y == 0) || (l.y == 47)) {
		if (is_hill_or_mountain(i - 1,j)) {
			if (is_hill_or_mountain(i + 1,j)) {
				ter_to_fix = 22;
				}
				else ter_to_fix = 26;
			}
			else ter_to_fix = 30;
	
		}
	else { // not edge

if (is_hill_or_mountain(i-1,j) == FALSE)
{
	if (is_hill_or_mountain(i,j-1) == FALSE)
	{
		ter_to_fix = 29;
	}
	else
	{
		if (is_hill_or_mountain(i,j+1) == FALSE)
			ter_to_fix = 31;
			else ter_to_fix = 30;
	}



}					

else // wall(i-1,j) == TRUE

{
	if (is_hill_or_mountain(i+1,j) == FALSE)
	{
		if (is_hill_or_mountain(i, j-1) == FALSE)
			ter_to_fix = 27;
			else if (is_hill_or_mountain(i,j+1) == FALSE)
				ter_to_fix = 25;
				else ter_to_fix = 26;
	
	}
	
	else //wall(i+1,j) == TRUE
	
	{
	if (is_hill_or_mountain(i, j-1) == FALSE)
		ter_to_fix = 28;
		else if (is_hill_or_mountain(i,j+1) == FALSE)
			ter_to_fix = 24;
			else if (is_hill_or_mountain(i-1,j-1) == FALSE)
				ter_to_fix = 33;
				else if(is_hill_or_mountain(i-1,j+1) == FALSE)
					ter_to_fix = 32;
					else if (is_hill_or_mountain(i+1,j-1) == FALSE)
						ter_to_fix = 34;
						else if (is_hill_or_mountain(i+1,j+1) == FALSE)
						ter_to_fix = 35;
						else ter_to_fix = 22; 
	
	
	
	}

}
}
			ter_to_fix += 14;
			//if ((ter_to_fix == 36) && (get_ran(1,0,15) == 5))
			//	ter_to_fix++; 
			if ((current_terrain.terrain[i][j] != 37) || (ter_to_fix != 36))
				current_terrain.terrain[i][j] = ter_to_fix; 				
			}

	if (store_ter == current_terrain.terrain[l.x][l.y])
		return FALSE;
		else return TRUE;

}

Boolean out_fix_water(location l)
{
	unsigned char ter_to_fix;
	unsigned char store_ter;
	short i,j;
	
	if ((l.x < 0) || (l.y > 47) || (l.y < 0) || (l.y > 47))
		return FALSE;
	i = l.x; j = l.y;
	store_ter = current_terrain.terrain[l.x][l.y];
	
	if (is_correctable_water(l.x,l.y)) {
	if (((l.x == 0) || (l.x == 47)) && ((l.y == 0) || (l.y == 47))) 
		ter_to_fix = 22;
	else if ((l.x == 0) || (l.x == 47)) {
		if (is_water(i,j-1)) {
			if (is_water(i,j+1)) {
				ter_to_fix = 22;
				}
				else ter_to_fix = 24;
			}
			else ter_to_fix = 28;
		}
	else if ((l.y == 0) || (l.y == 47)) {
		if (is_water(i - 1,j)) {
			if (is_water(i + 1,j)) {
				ter_to_fix = 22;
				}
				else ter_to_fix = 26;
			}
			else ter_to_fix = 30;
	
		}
	else { // not edge

if (is_water(i-1,j) == FALSE)
{
	if (is_water(i,j-1) == FALSE)
	{
		ter_to_fix = 29;
	}
	else
	{
		if (is_water(i,j+1) == FALSE)
			ter_to_fix = 31;
			else ter_to_fix = 30;
	}



}					

else // wall(i-1,j) == TRUE

{
	if (is_water(i+1,j) == FALSE)
	{
		if (is_water(i, j-1) == FALSE)
			ter_to_fix = 27;
			else if (is_water(i,j+1) == FALSE)
				ter_to_fix = 25;
				else ter_to_fix = 26;
	
	}
	
	else //wall(i+1,j) == TRUE
	
	{
	if (is_water(i, j-1) == FALSE)
		ter_to_fix = 28;
		else if (is_water(i,j+1) == FALSE)
			ter_to_fix = 24;
			else if (is_water(i-1,j-1) == FALSE)
				ter_to_fix = 33;
				else if(is_water(i-1,j+1) == FALSE)
					ter_to_fix = 32;
					else if (is_water(i+1,j-1) == FALSE)
						ter_to_fix = 34;
						else if (is_water(i+1,j+1) == FALSE)
						ter_to_fix = 35;
						else ter_to_fix = 22; 
	
	
	
	}

}
}
			ter_to_fix += 27;
			if (ter_to_fix == 49)
				ter_to_fix++;
			current_terrain.terrain[i][j] = ter_to_fix; 				
			}

	if (store_ter == current_terrain.terrain[l.x][l.y])
		return FALSE;
		else return TRUE;

}
// end out

void adjust_space(location l)
{
	Boolean needed_change = FALSE;
	location l2;
	short i,j;
	
	i = l.x; j = l.y;
	if ((editing_town == TRUE) && ((i < 0) || (i > max_dim[town_type] - 1) || (j < 0) || (j > max_dim[town_type] - 1)))
		return ;
	if ((editing_town == FALSE) && ((i < 0) || (i > 47) || (j < 0) || (j > 47)))
		return ;
		
	if (editing_town == TRUE) {
		if (fix_rubble(l) == TRUE)
			needed_change = TRUE;
		if (fix_cave(l) == TRUE)
			needed_change = TRUE;
		if (fix_mountain(l) == TRUE)
			needed_change = TRUE;
		if (fix_hill(l) == TRUE)
			needed_change = TRUE;
		if (fix_water(l) == TRUE)
			needed_change = TRUE;
		}
	if (editing_town == FALSE) {
		if (out_fix_rubble(l) == TRUE)
			needed_change = TRUE;
		if (out_fix_cave(l) == TRUE)
			needed_change = TRUE;
		if (out_fix_mountain(l) == TRUE)
			needed_change = TRUE;
		if (out_fix_hill(l) == TRUE)
			needed_change = TRUE;
		if (out_fix_water(l) == TRUE)
			needed_change = TRUE;
		}
		
	if (needed_change == TRUE) {
		l2 = l;
		l2.x--;
		adjust_space(l2);
		l2.x += 2;
		adjust_space(l2);
		l2.x--;
		l2.y--;
		adjust_space(l2);
		l2.y += 2;
		adjust_space(l2); 
		}

}

Boolean is_lava(short x,short y)
{
	if ((coord_to_ter(x,y) == 75) || (coord_to_ter(x,y) == 76))
		return TRUE;
		else return FALSE;
}

unsigned char coord_to_ter(short x,short y)
{
	unsigned char what_terrain;

	if (editing_town == TRUE)
		what_terrain = t_d.terrain[x][y];
		else what_terrain = current_terrain.terrain[x][y];

	return what_terrain;
}

short get_obscurity(short x,short y)
{
	unsigned char what_terrain;
	short store;
	
	what_terrain = coord_to_ter(x,y);
		
	store = scenario.ter_types[what_terrain].blockage;
	if ((store == 1) || (store == 5))
		return 5;
	if (store == 4)
		return 1;	
	return 0;
}


short can_see(location p1,location p2,short mode)
//mode; // 0 - normal  1 - counts 1 for blocked spaces or lava (used for party placement in
			//				   town combat)
			// 2 - no light check
{
	short dx,dy,count,storage = 0;

	if (p1.y == p2.y) {
		if (p1.x > p2.x) {
			for (count = p2.x + 1; count < p1.x; count++) {
				storage = storage + get_obscurity(count, p1.y);
				if (((scenario.ter_types[coord_to_ter(count,p1.y)].blockage > 2) || (is_lava(count,p1.y) == TRUE)) && (mode == 1))
					return 5;
				}
			}
			else {
				for (count = p1.x + 1; count < p2.x; count++) {

				storage = storage + get_obscurity(count, p1.y);
				if (((scenario.ter_types[coord_to_ter(count,p1.y)].blockage > 2) || (is_lava(count,p1.y) == TRUE)) && (mode == 1))
					return 5;
				}
			}
			return storage;
		}
	if (p1.x == p2.x) {
		if (p1.y > p2.y) {
			for (count = p1.y - 1; count > p2.y; count--) {
				storage = storage + get_obscurity(p1.x, count);
				if (((scenario.ter_types[coord_to_ter(p1.x,count)].blockage > 2) || (is_lava(p1.x,count) == TRUE)) && (mode == 1))
					return 5;
				}
			}
			else {
				for (count = p1.y + 1; count < p2.y; count++) {
					storage = storage + get_obscurity(p1.x, count);
					if (((scenario.ter_types[coord_to_ter(p1.x,count)].blockage > 2) || (is_lava(p1.x,count) == TRUE))  && (mode == 1))
						return 5;
					}
			}
		return storage;	
		}
	dx = p2.x - p1.x;
	dy = p2.y - p1.y;
	
	if (abs(dy) > abs(dx)) {
		if (p2.y > p1.y) {
			for (count = 1; count < dy; count++) {
				storage = storage + get_obscurity(p1.x + (count * dx) / dy, p1.y + count);
				if ( ((scenario.ter_types[coord_to_ter(p1.x + (count * dx) / dy,p1.y + count)].blockage > 2) ||
					(is_lava(p1.x + (count * dx) / dy,p1.y + count) == TRUE))
					 && (mode == 1))
					return 5;
				}			
			}
		else {
			for (count = -1; count > dy; count--) {
				storage = storage + get_obscurity(p1.x + (count * dx) / dy, p1.y + count);
				if ( ((scenario.ter_types[coord_to_ter(p1.x + (count * dx) / dy, p1.y + count)].blockage > 2) ||
					(is_lava(p1.x + (count * dx) / dy, p1.y + count) == TRUE))
					&& (mode == 1))
					return 5;				
				}
			}
		return storage;
		} 
	if (abs(dy) <= abs(dx)) {
		if (p2.x > p1.x) {
			for (count = 1; count < dx; count++) {
				storage = storage + get_obscurity(p1.x + count, p1.y + (count * dy) / dx);
				if (((scenario.ter_types[coord_to_ter(p1.x + count,p1.y + (count * dy) / dx)].blockage > 2) ||
					(is_lava(p1.x + count,p1.y + (count * dy) / dx) == TRUE))
					&& (mode == 1))
					return 5;
				}
			}
		else {
			for (count = -1; count > dx; count--) {
				storage = storage + get_obscurity(p1.x + count, p1.y + (count * dy) / dx);
				if ( ((scenario.ter_types[coord_to_ter(p1.x + count,p1.y + (count * dy) / dx)].blockage > 2) ||
					(is_lava(p1.x + count,p1.y + (count * dy) / dx) == TRUE))
					&& (mode == 1))
					return 5;
				}
			}
		return storage;
		} 
	if (storage > 5)
		return 5;
		else return storage;
}

Boolean place_item(location spot_hit,short which_item,short property,short always,short odds) 
// odds 0 - 100, with 100 always
{

	short x;

	if ((which_item < 0) || (which_item > 399))
		return TRUE;
	if (scen_item_list.scen_items[which_item].variety == 0)
		return TRUE;
	if (get_ran(1,1,100) > odds)
		return FALSE;
	for (x = 0; x < 64; x++) 
		if (town.preset_items[x].item_code < 0) {
			town.preset_items[x].item_loc.x = spot_hit.x;
			town.preset_items[x].item_loc.y = spot_hit.y;
			town.preset_items[x].item_code = which_item;
			town.preset_items[x].ability = -1;
			if ((scen_item_list.scen_items[which_item].variety == 3) ||
				(scen_item_list.scen_items[which_item].variety == 11))
					town.preset_items[x].ability = get_ran(1,4,6);

			//town.preset_items[x].charges = 0;
			town.preset_items[x].always_there = always;
			town.preset_items[x].property = property;
			town.preset_items[x].contained = container_there(town.preset_items[x].item_loc);
			return TRUE;
			}
	return FALSE;
}

void place_items_in_town()
{
	location l;
	short i,j,k,x;
	Boolean place_failed = FALSE;
	 
	for (i = 0; i < max_dim[town_type];i++)
		for (j = 0; j < max_dim[town_type];j++) {
			l.x = i; l.y = j;
			
			for (k = 0; k < 10; k++)
				if (t_d.terrain[i][j] == scenario.storage_shortcuts[k].ter_type) {
					for (x = 0; x < 10; x++)
						if (place_item(l,scenario.storage_shortcuts[k].item_num[x],
							scenario.storage_shortcuts[k].property,FALSE,
							scenario.storage_shortcuts[k].item_odds[x]) == FALSE)
								place_failed = TRUE;
					}
		 	}
	if (place_failed == TRUE)
		give_error("Not all of the random items could be placed. The preset item per town limit of 64 was reached.","",0);
	draw_terrain();
}

void place_edit_special(location loc)
{
	short i,spec;
	
	if (editing_town == TRUE) {
		for (i = 0; i < 50; i++)
			if ((town.special_locs[i].x == loc.x) && (town.special_locs[i].y == loc.y)) {
				edit_spec_enc(town.spec_id[i],2,0);
				i = 500;
				}
		if (i < 500) { // new special
			spec = get_fresh_spec(2);
			if (spec < 0) {
				give_error("You are out of special nodes in this town. Select Edit Special Nodes from the Town menu to clear out some of the special nodes.","",0);
				return;
				}
			for (i = 0; i < 50; i++)
				if (town.special_locs[i].x == 100) {
					town.special_locs[i] = loc;
					town.spec_id[i] = spec;
					edit_spec_enc(spec,2,0);
					
					if (town.specials[spec].pic < 0)
						town.special_locs[i].x = 100;
					i = 500;
					}
			if (i < 500) {
				give_error("Each town can have at most 50 locations with special encounters. You'll need to erase some special spaces before you can place more.","",0);
				return;			
				}
			}
		}

	if (editing_town == FALSE) {
		if ((loc.x == 0) || (loc.x == 47) || (loc.y == 0) || (loc.y == 47)) {
			fancy_choice_dialog(870,0);
			return;
			}
		for (i = 0; i < 18; i++)
			if ((current_terrain.special_locs[i].x == loc.x) && (current_terrain.special_locs[i].y == loc.y)) {
				edit_spec_enc(current_terrain.special_id[i],1,0);
				i = 500;
				}
		if (i < 500) { // new special
			spec = get_fresh_spec(1);
			if (spec < 0) {
				give_error("You are out of special nodes in this outdoor section. Select Edit Special Nodes from the Outdoor menu to clear out some of the special nodes.","",0);
				return;
				}
			for (i = 0; i < 18; i++)
				if (current_terrain.special_locs[i].x == 100) {
					current_terrain.special_locs[i] = loc;
					current_terrain.special_id[i] = spec;
					edit_spec_enc(spec,1,0);
					
					if (current_terrain.specials[spec].pic < 0)
						current_terrain.special_locs[i].x = 100;
					i = 500;
					}
			if (i < 500) {
				give_error("Each outdoor can have at most 18 locations with special encounters. You'll need to erase some special spaces before you can place more.","",0);
				return;			
				}
			}
		}

}

void set_special(location spot_hit)
{
	short x,y;
	
				if (editing_town == TRUE) {
					for (x = 0; x < 50; x++)
						if ((town.special_locs[x].x == spot_hit.x) && (town.special_locs[x].y == spot_hit.y)) {
							y = edit_special_num(2,town.spec_id[x]);
							if (y >= 0) town.spec_id[x] = y;
							x = 500;
							}
					if (x < 500) {
						for (x = 0; x < 50; x++)
							if (town.special_locs[x].x == 100) {
								y = edit_special_num(2,0);
								if (y >= 0) {
									town.special_locs[x] = spot_hit;
									town.spec_id[x] = y;
									}
								x = 500;
								}
						if (x < 500)
							give_error("Each town can have at most 50 locations with special encounters. Each outdoor section can have at most 18. You'll need to erase some special spaces before you can place more.","",0);
						}
					}
				if (editing_town == FALSE) {
					if ((spot_hit.x == 0) || (spot_hit.x == 47) || (spot_hit.y == 0) || (spot_hit.y == 47)) {
						fancy_choice_dialog(870,0);
						return;
						}
					for (x = 0; x < 18; x++)
						if ((current_terrain.special_locs[x].x == spot_hit.x) && (current_terrain.special_locs[x].y == spot_hit.y)) {
							y = edit_special_num(1,current_terrain.special_id[x]);
							if (y >= 0) current_terrain.special_id[x] = y;
							x = 500;
							}
					if (x < 500) {
						for (x = 0; x < 18; x++)
							if (current_terrain.special_locs[x].x == 100) {
								y = edit_special_num(1,current_terrain.special_id[x]);
								if (y >= 0) {
									current_terrain.special_locs[x] = spot_hit;
									current_terrain.special_id[x] = y;
									}
								x = 500;
								}
						if (x < 500)
							give_error("Each town can have at most 50 locations with special encounters. Each outdoor section can have at most 18. You'll need to erase some special spaces before you can place more.","",0);
						}
					}

}

void town_entry(location spot_hit)
{
	short x,y;
	unsigned char ter;
	
	ter = current_terrain.terrain[spot_hit.x][spot_hit.y];
	if (scenario.ter_types[ter].special != 21) {
		give_error("This space isn't a town entrance. Town entrances are marked by a small brown castle icon.","",0);
		return;
		}
	// clean up old town entrys
	for (x = 0; x < 8; x++)
		if (current_terrain.exit_locs[x].x < 100) {
			ter = current_terrain.terrain[current_terrain.exit_locs[x].x][current_terrain.exit_locs[x].y];
			if (scenario.ter_types[ter].special != 21) 
				current_terrain.exit_locs[x].x = 100;
			}
				y = -2;
				for (x = 0; x < 8; x++)
					if ((current_terrain.exit_locs[x].x == spot_hit.x) && (current_terrain.exit_locs[x].y == spot_hit.y)) {
						y = pick_town_num(856,current_terrain.exit_dests[x]);
						if (y >= 0) current_terrain.exit_dests[x] = y;
						}
				if (y == -2) {
					for (x = 0; x < 8; x++)
						if (current_terrain.exit_locs[x].x == 100) {
							y = pick_town_num(856,0);
							if (y >= 0) {
								current_terrain.exit_dests[x] = y;
								current_terrain.exit_locs[x] = spot_hit;							
								}
							x = 500;
							}
					if (y == -2)
						give_error("You can't set more than 8 town entrances in any outdoor section.","",0);
					}
}

// is slot >= 0, force that slot
// if -1, use 1st free slot
void set_up_start_screen()
{
	set_lb(0,2,"Blades of Exile",0);
	set_lb(1,2,"Scenario Editor",0);
	//set_lb(3,2,"by Jeff Vogel",0);
	set_lb(3,11,"Make New Scenario",0);
	set_lb(4,11,"Load Scenario",0);
	set_lb(7,1,"To find out how to use the",0);
	set_lb(8,1,"editor, select Getting Started ",0);
	set_lb(9,1,"from the Help menu.",0);
	set_lb(NLS - 5,1,"Be sure to read the file Blades",0);
	set_lb(NLS - 4,1,"of Exile License. Using this",0);
	set_lb(NLS - 3,1,"program implies that you agree ",0);
	set_lb(NLS - 2,1,"with the terms of the license.",0);
	//set_lb(NLS - 2,1,"Copyright 1997, Spiderweb Software",0);
	//set_lb(NLS - 1,1,"All rights reserved.",0);
	set_lb(NLS - 1,1,"Copyright 1997, All rights reserved.",0);
}

void set_up_main_screen()
{
	char message[50];
	
	reset_lb();
	reset_rb();
	set_lb(-1,2,"Blades of Exile",0);
	set_lb(-1,1,"Scenario Options",0);
	set_lb(-1,11,"Edit Terrain Types",0);
	set_lb(-1,11,"Edit Monsters",0);
	set_lb(-1,11,"Edit Items",0);
	set_lb(-1,11,"Create New Town",0);
	set_lb(-1,11,"Edit Scenario Text",0);
	set_lb(-1,11,"Edit Special Items",0);
	set_lb(-1,1,"",0);
	set_lb(-1,1,"Outdoors Options",0);
	sprintf((char *) message,"  Section x = %d, y = %d",(short) cur_out.x,(short) cur_out.y);
	set_lb(-1,1,(char *) message,0);
	set_lb(-1,11,"Load New Section",0);
	set_lb(-1,11,"Edit Outdoor Terrain",0);
	set_lb(-1,1,"",0);
	set_lb(-1,1,"Town/Dungeon Options",0);
	sprintf((char *) message,"  Town %d: %s",cur_town,town_strs[0]);
	set_lb(-1,1,(char *) message,0);
	set_lb(-1,11,"Load Another Town",0);
	set_lb(-1,11,"Edit Town Terrain",0);
	set_lb(-1,11,"Edit Town Dialogue",0);
	//set_lb(NLS - 2,1,"Copyright 1997",0);
	set_lb(NLS - 1,1,"Copyright 1997, All rights reserved.",0);
	overall_mode = 60;
	ShowScrollBar(right_sbar,SB_CTL,TRUE);
	shut_down_menus(4);
	shut_down_menus(3);
	//DrawMenuBar();
	redraw_screen();
	//draw_main_screen();
}

void start_town_edit()
{
	short i,j;
	char message[50];
	small_any_drawn = FALSE;
	cen_x = max_dim[town_type] / 2; cen_y = max_dim[town_type] / 2;
	reset_lb();
	set_lb(0,2,"Editing town",0);
	sprintf((char *) message,"Editing Town %d: %s",cur_town,town_strs[0]);
	set_lb(NLS -2,1,(char *) message,0);
	set_lb(NLS - 1,11,"Back to Main Menu",0);
	overall_mode = 0;
	editing_town = TRUE;
	set_up_terrain_buttons();
	shut_down_menus(4);
	shut_down_menus(2);
	//DrawMenuBar();
	ShowScrollBar(right_sbar,SB_CTL,FALSE);
	redraw_screen();
	set_string("Drawing mode","");
	place_location();
	copied_spec = -1;
	for (i = 0; i < max_dim[town_type]; i++)
		for (j = 0; j < max_dim[town_type]; j++)
			if (t_d.terrain[i][j] == 0)
				current_ground = 0;
				else if (t_d.terrain[i][j] == 2)
					current_ground = 2;
}

void start_out_edit()
{
	short i,j;
	char message[50];
	char out_strs[0];
	small_any_drawn = FALSE;
	cen_x = 24; cen_y = 24;
	reset_lb();
	set_lb(0,2,"Editing outdoors",0);
	sprintf((char *) message,"X = %d, Y = %d: %s",(short) cur_out.x,(short) cur_out.y,data_store->out_strs[0]);
	set_lb(NLS -2,1,(char *) message,0);
	set_lb(NLS - 1,11,"Back to Main Menu",0);
	overall_mode = 0;
	editing_town = FALSE;
	set_up_terrain_buttons();
	ShowScrollBar(right_sbar,SB_CTL,FALSE);
	shut_down_menus(4);
	shut_down_menus(1);
	//DrawMenuBar();
	redraw_screen();
	set_string("Drawing mode","");
	place_location();
	copied_spec = -1;
		for (i = 0; i < 48; i++)
			for (j = 0; j < 48; j++)
				if (current_terrain.terrain[i][j] == 0)
					current_ground = 0;
					else if (current_terrain.terrain[i][j] == 2)
						current_ground = 2;
}

// mode 0 - initial shut down, 1 - no town, 2 - no out, 3 - no town or out 4 - all menus on
void shut_down_menus(short mode)
{
	HMENU menu;
	short i;
	short scen_items[18] = {101,103,104,105,106, 109,110,112,113,114, 115,116,117,118,119, 120,111, 121};
	short town_items[17] = {201,202,203,204,205, 206,208,209,210,213, 214,215,216, 217,218,219,220};
	short out_items[10] = {301,302,303,304,305,306,308,311,312,313};
	menu = GetMenu(mainPtr);
		//menu[i] = GetSubMenu(big_menu,5 + i);

	if (mode == 0) {
		EnableMenuItem(menu,2,MF_GRAYED | MF_BYCOMMAND);
		for (i = 0; i < 18; i++)
			EnableMenuItem(menu,scen_items[i],MF_GRAYED | MF_BYCOMMAND);
		for (i = 0; i < 17; i++)
			EnableMenuItem(menu,town_items[i],MF_GRAYED | MF_BYCOMMAND);
		for (i = 0; i < 10; i++)
			EnableMenuItem(menu,out_items[i],MF_GRAYED | MF_BYCOMMAND);
        for (i = 0; i < 10; i++)
           	EnableMenuItem(menu,401+i,MF_GRAYED | MF_BYCOMMAND);
		//for (i = 600; i < 1000; i++) {
		//	EnableMenuItem(menu,i,MF_GRAYED | MF_BYCOMMAND);
		//	}
		//for (i = 1000; i < 1256; i++) {
		//	EnableMenuItem(menu,i,MF_GRAYED | MF_BYCOMMAND);
		//	}
		}
	if  (mode == 4) {
		EnableMenuItem(menu,1,MF_ENABLED | MF_BYCOMMAND);
		EnableMenuItem(menu,2,MF_ENABLED | MF_BYCOMMAND);
		EnableMenuItem(menu,3,MF_ENABLED | MF_BYCOMMAND);
		EnableMenuItem(menu,5,MF_ENABLED | MF_BYCOMMAND);
		for (i = 0; i < 18; i++)
			EnableMenuItem(menu,scen_items[i],MF_ENABLED | MF_BYCOMMAND);
		for (i = 0; i < 17; i++)
			EnableMenuItem(menu,town_items[i],MF_ENABLED | MF_BYCOMMAND);
		for (i = 0; i < 10; i++)
			EnableMenuItem(menu,out_items[i],MF_ENABLED | MF_BYCOMMAND);
        for (i = 0; i < 10; i++)
           	EnableMenuItem(menu,401+i,MF_ENABLED | MF_BYCOMMAND);			
		//for (i = 600; i < 1000; i++) {
		 //	EnableMenuItem(menu,i,MF_ENABLED | MF_BYCOMMAND);
		//	}
		//for (i = 1000; i < 1256; i++) {
		//	EnableMenuItem(menu,i,MF_ENABLED | MF_BYCOMMAND);
		//	}

		}
	if ((mode == 1) || (mode == 3)) {
		for (i = 0; i < 9; i++)
			EnableMenuItem(menu,town_items[i],MF_GRAYED | MF_BYCOMMAND);
		//for (i = 600; i < 1000; i++) {
		//	EnableMenuItem(menu,i,MF_GRAYED | MF_BYCOMMAND);
		//	}
		//for (i = 1000; i < 1256; i++) {
		//	EnableMenuItem(menu,i,MF_GRAYED | MF_BYCOMMAND);
		//	}
		}
	if ((mode == 2) || (mode == 3)) {
		for (i = 0; i < 7; i++)
			EnableMenuItem(menu,out_items[i],MF_GRAYED | MF_BYCOMMAND);
		}
}

void start_terrain_editing()
{
	short i;
	char str[256];
	
	ShowScrollBar(right_sbar,SB_CTL,FALSE);
	overall_mode = 62;
	set_up_terrain_buttons();
	place_location();
		
	set_lb(NLS - 3,0,"",1);
/*	SetControlValue(right_sbar,0);
	
	reset_rb();
	SetControlMaximum(right_sbar,256 - NRSONPAGE);
	for (i = 0; i < 256; i++) {
		sprintf((char *) str,"%d - %s",i,(char *) scen_item_list.ter_names[i]);
		set_rb(i,1000 + i,(char *) str,0);
		}
	for (i = 0; i < NRSONPAGE; i++)
		draw_rb_slot(i); */
}

void start_monster_editing(short just_redo_text)
{
	short i;
	char str[256];
	Boolean draw_full = FALSE;

	if (overall_mode == 62)
		draw_full = TRUE;
	if (just_redo_text == 0) {
		overall_mode = 60;
		ShowScrollBar(right_sbar,SB_CTL,TRUE);
		SetScrollPos(right_sbar,SB_CTL,0,TRUE);
	
		reset_rb();
		SetScrollRange(right_sbar,SB_CTL,0,255 - NRSONPAGE,TRUE);
		}
	for (i = 0; i < 256; i++) {
		sprintf((char *) str,"%d - %s",i,(char *) scen_item_list.monst_names[i]);
		set_rb(i,2000 + i,(char *) str,0);
		}
	if (draw_full == TRUE)
		redraw_screen();
		else for (i = 0; i < NRSONPAGE; i++)
		draw_rb_slot(i,0); 
	set_lb(NLS - 3,0,"",1);
}

Boolean monst_on_space(location loc,short m_num)
{
	short i,j;
	
	if (editing_town == FALSE)
		return FALSE;
	if (t_d.creatures[m_num].number == 0)
		return FALSE;
	if ((loc.x - t_d.creatures[m_num].start_loc.x >= 0) && 
	(loc.x - t_d.creatures[m_num].start_loc.x <= scenario.scen_monsters[t_d.creatures[m_num].number].x_width - 1) &&
	(loc.y - t_d.creatures[m_num].start_loc.y >= 0) && 
	(loc.y - t_d.creatures[m_num].start_loc.y <= scenario.scen_monsters[t_d.creatures[m_num].number].y_width - 1))
		return TRUE;
	return FALSE;	
	
}

void start_item_editing(short just_redo_text)
{
	short i;
	char str[256];
	Boolean draw_full = FALSE;
	
	if (just_redo_text == 0) {
		if (overall_mode < 60)
			set_up_main_screen();
		if (overall_mode == 62)
			draw_full = TRUE;
		overall_mode = 60;
		ShowScrollBar(right_sbar,SB_CTL,TRUE);
		SetScrollPos(right_sbar,SB_CTL,0,TRUE);
	
		reset_rb();
		SetScrollRange(right_sbar,SB_CTL,0,400 - NRSONPAGE,TRUE);
		}
	for (i = 0; i < 400; i++) {
		sprintf((char *) str,"%d - %s",i,(char *) scen_item_list.scen_items[i].full_name);
		set_rb(i,3000 + i,(char *) str,0);
		}
	if (draw_full == TRUE)
		redraw_screen();
		else for (i = 0; i < NRSONPAGE; i++)
			draw_rb_slot(i,0); 
	set_lb(NLS - 3,0,"",1);
}

void start_special_item_editing()
{
	short i;
	char str[256];
	Boolean draw_full = FALSE;

	if (overall_mode < 60)
		set_up_main_screen();
	if (overall_mode == 62)
		draw_full = TRUE;
	overall_mode = 60;
		SetScrollPos(right_sbar,SB_CTL,0,TRUE);
	
		SetScrollRange(right_sbar,SB_CTL,0,50 - NRSONPAGE,TRUE);
		reset_rb();
	for (i = 0; i < 50; i++) {
		sprintf((char *) str,"%d - %s",i,(char *) scen_strs[60 + i * 2]);
		set_rb(i,10000 + i,(char *) str,0);
		}
	if (draw_full == TRUE)
		redraw_screen();
		else for (i = 0; i < NRSONPAGE; i++)
		draw_rb_slot(i,0); 
	set_lb(NLS - 3,0,"",1);
}

// mode 0 - scen 1 - out 2 - town
// if just_redo_text not 0, simply need to update text portions
void start_string_editing(short mode,short just_redo_text)
{
	short i,pos;
	char str[256];
	short num_strs[3] = {260,108,140};
	Boolean draw_full = FALSE;

	if (just_redo_text == 0) {
		if (overall_mode < 60)
			set_up_main_screen();
		if (overall_mode == 62)
			draw_full = TRUE;
		overall_mode = 60;
		ShowScrollBar(right_sbar,SB_CTL,TRUE);
		SetScrollPos(right_sbar,SB_CTL,0,TRUE);
	
		reset_rb();
		SetScrollRange(right_sbar,SB_CTL,0,num_strs[mode] - NRSONPAGE,TRUE);

		}
	for (i = 0; i < num_strs[mode]; i++) {
		switch (mode) {
			case 0:
				if (i < 160)
					sprintf((char *) str,"%d - %-30.30s",i,(char *) scen_strs[i]);
					else sprintf((char *) str,"%d - %-30.30s",i,(char *) scen_strs2[i - 160]);
				set_rb(i,7000 + i,(char *) str,0);
				break;
			case 1:
				sprintf((char *) str,"%d - %-30.30s",i,(char *) data_store->out_strs[i]);
				set_rb(i,8000 + i,(char *) str,0);
				break;
			case 2:
				sprintf((char *) str,"%d - %-30.30s",i,(char *) town_strs[i]);
				set_rb(i,9000 + i,(char *) str,0);
				break;
			}
		}
	
	pos = GetScrollPos(right_sbar,SB_CTL);
	if (draw_full == TRUE)
		redraw_screen();
		else for (i = 0; i < NRSONPAGE; i++)
		draw_rb_slot(i + pos,0); 
	set_lb(NLS - 3,1,"Control-click to clear",1);
}

// mode 0 - scen 1 - out 2 - town
// if just_redo_text not 0, simply need to update text portions
void start_special_editing(short mode,short just_redo_text)
{
	short i;
	char str[256],s2[256];
	short num_specs[3] = {256,60,100};
	Boolean draw_full = FALSE;

	if (just_redo_text == 0) {
		if (overall_mode < 60)
			set_up_main_screen();
		if (overall_mode == 62)
			draw_full = TRUE;
		overall_mode = 60;
		ShowScrollBar(right_sbar,SB_CTL,TRUE);
	
		reset_rb();
		SetScrollRange(right_sbar,SB_CTL,0,num_specs[mode] - NRSONPAGE,TRUE);

		}
		
	for (i = 0; i < num_specs[mode]; i++) {
		switch (mode) {
			case 0:
				get_str(s2,22,scenario.scen_specials[i].type + 1);
				sprintf((char *) str,"%d - %-30.30s",i,(char *) s2);
				set_rb(i,4000 + i,(char *) str,0);
				break;
			case 1:
				get_str(s2,22,current_terrain.specials[i].type + 1);
				sprintf((char *) str,"%d - %-30.30s",i,(char *) s2);
				set_rb(i,5000 + i,(char *) str,0);
				break;
			case 2:
				get_str(s2,22,town.specials[i].type + 1);
				sprintf((char *) str,"%d - %-30.30s",i,(char *) s2);
				set_rb(i,6000 + i,(char *) str,0);
				break;
			}
		}
	if (draw_full == TRUE)
		redraw_screen();
		else for (i = 0; i < NRSONPAGE; i++)
		draw_rb_slot(i,0); 
	set_lb(NLS - 3,1,"Control-click to clear",1);
}

// if restoring is 1, this is just a redraw, so don't move scroll bar position
void start_dialogue_editing(short restoring)
{
	short i,j;
	char str[256];
	char s[15] = "    ,      ";
	Boolean draw_full = FALSE;
	
	if (overall_mode < 60)
		set_up_main_screen();
	if (overall_mode == 62)
		draw_full = TRUE;
	overall_mode = 60;
	ShowScrollBar(right_sbar,SB_CTL,TRUE);
	
	if (restoring == 0) {
		SetScrollPos(right_sbar,SB_CTL,0,TRUE);
		reset_rb();
		SetScrollRange(right_sbar,SB_CTL,0,70 - NRSONPAGE,TRUE);
		}
	for (i = 0; i < 10; i++) {
		sprintf((char *) str,"Personality %d - %s",i + cur_town * 10,
			(char *) talk_strs[i]);
		set_rb(i,13000 + i,(char *) str,0);
		}
	for (i = 0; i < 60; i++) {
		for (j = 0; j < 4; j++) {
			s[j] = talking.talk_nodes[i].link1[j];
			s[j + 6] = talking.talk_nodes[i].link2[j];
			}
		sprintf((char *) str,"Node %d - Per. %d, %s",i,
			talking.talk_nodes[i].personality,s);
		set_rb(10 + i,12000 + i,(char *) str,0);
		}
	if (draw_full == TRUE)
		redraw_screen();
		else for (i = 0; i < NRSONPAGE; i++)
		draw_rb_slot(i,0); 
	set_lb(NLS - 3,0,"",1);
}

Boolean save_check(short which_dlog)
{
	short choice;
	
	if (change_made == FALSE)
		return TRUE;
	change_made = FALSE;
	choice = fancy_choice_dialog(which_dlog,0);
	if (choice == 3)
		return TRUE;
	if (choice == 2)
		return FALSE;
	modify_lists();
	save_scenario();
	return TRUE;
}

void update_item_menu()
{
	short i,j;
	HMENU menu[13],big_menu;
	char item_name[256];

	big_menu = GetMenu(mainPtr);

	for (i = 0; i < 13; i++)
		menu[i] = GetSubMenu(big_menu,5 + i);

//	for (i = 0; i < 400; i++)
//		DeleteMenu(big_menu,600 + i,MF_BYCOMMAND);

	for (j = 0; j < 8; j++) {
		for (i = 0; i < 50; i++) {
			DeleteMenu(menu[j],0,MF_BYPOSITION);
			}
		if (overall_mode < 61) {
			for (i = 0; i < 50; i++) {
					sprintf((char *) item_name, "%d - %s",i + j * 50,scen_item_list.scen_items[i + j * 50].full_name);
					if ((i % 25 == 0) && (i > 0))
						AppendMenu(menu[j],MF_MENUBREAK | MF_BYCOMMAND | MF_ENABLED | MF_STRING, 600 + (50 * j) + i, item_name);
						else AppendMenu(menu[j],MF_BYCOMMAND | MF_ENABLED | MF_STRING, 600 + (50 * j) + i, item_name);
					}
			}
		}
		
	for (j = 0; j < 5; j++) {
		for (i = 0; i < 51; i++) {
			DeleteMenu(menu[j + 8],0,MF_BYPOSITION);
			}
		if (overall_mode < 61) {
			for (i = 0; i < 51; i++) {
					sprintf((char *) item_name, "%d - %s",(51 * j) + i + 1,scen_item_list.monst_names[(51 * j) + i + 1]);
					if ((i % 26 == 0) && (i > 0))
						AppendMenu(menu[j + 8],MF_MENUBREAK | MF_BYCOMMAND | MF_ENABLED | MF_STRING, 1001 + (51 * j) + i, item_name);
						else AppendMenu(menu[j + 8],MF_BYCOMMAND | MF_ENABLED | MF_STRING, 1001 + (51 * j) + i, item_name);
					}
				}
			}
/*
	for (j = 0; j < 4; j++) {
		for (i = 0; i < 50; i++) {
			DeleteMenu(menu[j + 13],0,MF_BYPOSITION);
			}
		if (overall_mode < 61) {
			for (i = 0; i < 50; i++) {
					if ((i % 25 == 0) && (i > 0))
						AppendMenu(menu[j + 13],MF_MENUBREAK | MF_BYCOMMAND | MF_ENABLED | MF_STRING,1400 +  (50 * j) + i, item_name);
					else
						AppendMenu(menu[j + 13],MF_BYCOMMAND | MF_ENABLED | MF_STRING, 1400 + (50 * j) + i, item_name);
					if ((i + 50 * j) < scenario.num_towns) {
					load_town(i + 50 * j);
					sprintf((char *) item_name, "%d - %s",i + j * 50,town_strs[0]);
				}
					if ((i + 50 * j) >= scenario.num_towns) {
					sprintf((char *) item_name, "%d - Town does not exist",i + j * 50);
				}
					}
				}
	}

*/
}

