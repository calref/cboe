#define  LINES_IN_TEXT_WIN	11
#define	TEXT_BUF_LEN	70

#include <stdio.h>
#include <string.h>

//#include "item.h"

#include "boe.global.h"
#include "boe.text.h"
#include "boe.locutils.h"
#include "boe.fields.h"
#include "mathutil.h"
#include "graphtool.h"
//#include "soundtool.h"
				

char *m_mage_sp[] = {"Spark","Minor Haste","Strength","Flame Cloud","Flame",
						"Minor Poison","Slow","Dumbfound","Stinking Cloud","Summon Beast",
						"Conflagration","Fireball","Weak Summoning","Web","Poison",
						"Ice Bolt","Slow Group","Major Haste","Firestorm","Summoning",
						"Shockstorm","Major Poison","Kill","Daemon","Major Blessing",
						"Major Summoning","Shockwave"};
char *m_priest_sp[] = {"Minor Bless","Light Heal","Wrack","Stumble","Bless",
						"Curse","Wound","Summon Spirit","Disease","Heal",
						"Holy Scourge","Smite","Curse All","Sticks to Snakes","Martyr's Shield",
						"Bless All","Major Heal","Flamestrike","Summon Host","Heal All",
						"Unholy Ravaging","Summon Guardian","Pestilence","Revive All","Avatar",
						"Divine Thud"};
				
typedef struct {
	char line[50];
	} buf_line; 

buf_line text_buffer[TEXT_BUF_LEN];
short buf_pointer = 30, lines_to_print= 0, num_added_since_stop = 0;
char store_string[60],store_string2[60];
short start_print_point= 0;
short mark_where_printing_long;
Boolean printing_long = FALSE;
Str255 c_str = "";
Boolean save_mess_given = FALSE;

Rect status_panel_clip_rect = {11, 299, 175, 495},item_panel_clip_rect = {11,297,175,463};

Rect item_buttons_from[7] = {{11,0,24,14},{11,14,24,28},{11,28,24,42},{11,42,24,56},
						{24,0,36,30},{24,30,36,60},{36,0,48,30}};

short store_mode;

extern short had_text_freeze,stat_screen_mode;

// graphics globals
extern Rect status_panel_rect,status_panel_title_rect;
extern Rect	text_panel_rect;
extern short overall_mode,which_combat_type,stat_window;
extern WindowPtr mainPtr;
extern Rect more_info_button;
extern short which_item_page[6],current_cursor;
//extern CursHandle sword_curs;
extern ControlHandle text_sbar,item_sbar;
extern Point store_anim_ul;
extern PixPatHandle	bg[];
extern short dest_personalities[40];
extern location source_locs[6];
extern location dest_locs[40] ;
//extern piles_of_stuff_dumping_type *data_store;
extern cScenario scenario;

extern GWorldPtr spec_scen_g,mixed_gworld, pc_stats_gworld, item_stats_gworld, text_area_gworld,tiny_obj_gworld,party_template_gworld;
extern short terrain_there[9][9];

// game globals
extern party_record_type party;
extern current_town_type c_town;
extern outdoor_record_type outdoors[2][2];
extern town_item_list	t_i;
extern unsigned char out[96][96];
extern pc_record_type adven[6];
extern big_tr_type t_d;
extern Point ul;
extern Boolean play_sounds,suppress_stat_screen,cartoon_happening,in_startup_mode;
extern Rect item_buttons[8][6];
 // name, use, give, drip, info, sell/id
extern Rect pc_buttons[6][5];
 // name, hp, sp, info, trade
extern Boolean item_area_button_active[8][6];
extern Boolean pc_area_button_active[6][5];
extern Rect item_screen_button_rects[9];
extern short spec_item_array[60];
extern short abil_chart[200],store_anim_type;
// combat globals
extern short item_bottom_button_active[9];

extern location pc_pos[6];
extern unsigned char combat_terrain[64][64];
extern short current_pc;
extern short shop_identify_cost;
extern short store_selling_values[8];
extern short combat_posing_monster, current_working_monster; // 0-5 PC 100 + x - monster x
extern Boolean supressing_some_spaces;
extern location ok_space[4];

short text_pc_has_abil_equip(short pc_num,short abil)
{
	short i = 0;
	
	while (((adven[pc_num].items[i].variety == 0) || (adven[pc_num].items[i].ability != abil)
			|| (adven[pc_num].equip[i] == FALSE)) && (i < 24))
				i++;
	return i;
				
}

// Draws the pc area in upper right
//void win_draw_string(WindowPtr dest_window,Rect dest_rect,char *str,short mode,short line_height)
void put_pc_screen()
{
	Str255 to_draw;
	short i = 0,j;
	Rect erase_rect = {17,2,98,269},to_draw_rect,from_rect; 
	Rect small_erase_rects[3] = {{101,34,114,76},{101,106,114,147},{101,174,114,201}};
	GrafPtr old_port;
	RGBColor store_color;
	Rect info_from = {0,1,12,13};

	GetBackColor(&store_color);

	// Now place new stuff. Just draw it all there naively. It's in a gworld, and fast, so
	// who gives a shit?
	GetPort(&old_port);	
	SetPort( pc_stats_gworld);

	// First clean up gworld with pretty patterns
	FillCRect(&erase_rect,bg[6]);
	for (i = 0; i < 3; i++)
		FillCRect(&small_erase_rects[i],bg[7]);
	
	ForeColor(whiteColor);
	// Put food, gold, day
	sprintf((char *) to_draw, "%d", (short) party.gold);		
	win_draw_string( pc_stats_gworld,small_erase_rects[1],
 	  to_draw,0,10,false);
	sprintf((char *) to_draw, "%d", (short) party.food);		
	win_draw_string( pc_stats_gworld,small_erase_rects[0],
 	  to_draw,0,10,false);
	i = calc_day();
	sprintf((char *) to_draw, "%d", i);		
	win_draw_string( pc_stats_gworld,small_erase_rects[2],
 	  to_draw,0,10,false);
	ForeColor(blackColor);
	
	for (i = 0; i < 6; i++) {
		if (adven[i].main_status != 0) {
			for (j = 0; j < 5; j++)
				pc_area_button_active[i][j] = 1;
			if (i == current_pc) {
				TextFace(italic | bold);
				ForeColor(blueColor);
				}

			sprintf((char *) to_draw, "%d. %-20s             ", i + 1, (char *) adven[i].name);		
			win_draw_string( pc_stats_gworld,pc_buttons[i][0],
 			 to_draw,0,10,false);
			TextFace(0);
 			TextFace(bold);
			ForeColor(blackColor);
	
			to_draw_rect = pc_buttons[i][1];
			to_draw_rect.right += 20;
			switch (adven[i].main_status) {
				case 1:
					if (adven[i].cur_health == adven[i].max_health) 
						ForeColor(greenColor);
						else ForeColor(redColor);
					sprintf((char *) to_draw, "%-3d              ",adven[i].cur_health);
					win_draw_string( pc_stats_gworld,pc_buttons[i][1],
 			 		  to_draw,0,10,false);
					if (adven[i].cur_sp == adven[i].max_sp) 
						ForeColor(blueColor);
						else ForeColor(magentaColor);
					sprintf((char *) to_draw, "%-3d              ",adven[i].cur_sp);
					win_draw_string( pc_stats_gworld,pc_buttons[i][2],
 			 		  to_draw,0,10,false);
					ForeColor(blackColor);
					draw_pc_effects(i);
					break;
				case 2:
					sprintf((char *) to_draw, "Dead");
					break;
				case 3:
					sprintf((char *) to_draw, "Dust");
					break;
				case 4:
					sprintf((char *) to_draw, "Stone");
					break;
				case 5:
					sprintf((char *) to_draw, "Fled");
					break;
				case 6:
					sprintf((char *) to_draw, "Surface");
					break;
				case 7:
					sprintf((char *) to_draw, "Won");
					break;
				default:
					sprintf((char *) to_draw, "Absent");
					break;
				}
			if (adven[i].main_status != 1)
				win_draw_string( pc_stats_gworld,to_draw_rect,
 			 	 to_draw,0,10,false);
			
			// Now put trade and info buttons
			//rect_draw_some_item(mixed_gworld,info_from,pc_stats_gworld,pc_buttons[i][3],1,0);
			//rect_draw_some_item(mixed_gworld,switch_from,pc_stats_gworld,pc_buttons[i][4],1,0);
			// do faster!
			to_draw_rect = pc_buttons[i][3];
			to_draw_rect.right = pc_buttons[i][4].right + 1;
			from_rect = info_from;
			from_rect.right = from_rect.left + to_draw_rect.right - to_draw_rect.left;
			rect_draw_some_item(mixed_gworld,from_rect,pc_stats_gworld,to_draw_rect,1,0);
			}
			else {
				for (j = 0; j < 5; j++)
					pc_area_button_active[i][j] = 0;
				}
		}
	// Now put text on window.
	SetPort(GetWindowPort(mainPtr));
	BackColor(whiteColor);
	GetPortBounds(pc_stats_gworld, &to_draw_rect);
	Rect oldRect = to_draw_rect;
	OffsetRect(&to_draw_rect,PC_WIN_UL_X,PC_WIN_UL_Y);
	rect_draw_some_item (pc_stats_gworld, oldRect, pc_stats_gworld, to_draw_rect, 0, 1);
	
	SetPort(old_port);	
	RGBBackColor(&store_color);

	// Sometimes this gets called when character is slain. when that happens, if items for
	// that PC are up, switch item page.
	if ((current_pc < 6) && (adven[current_pc].main_status != 1) && (stat_window == current_pc)) {
		set_stat_window(current_pc);
		}
}

// Draws item area in middle right
// Screen_num is what page is visible in the item menu. 
//    0 - 5 pc inventory  6 - special item  7 - missions
// Stat_screen_mode ... 0 - normal, adventuring, all buttons vis
//						1 - in shop, item info only
//						2 - in shop, identifying, shop_identify_cost is cost
//						3 - in shop, selling weapons
//						4 - in shop, selling armor
//						5 - in shop, selling all
//						6 - in shop, augmenting weapon,shop_identify_cost is type
void put_item_screen(short screen_num,short suppress_buttons)
// if suppress_buttons > 0, save time by not redrawing buttons
{
	Str255 to_draw;
	short i_num,item_offset;
	short i = 0,j,pc;
	Rect erase_rect = {17,2,122,255},dest_rect;
	Rect upper_frame_rect = {3,3,15,268};
	GrafPtr old_port;
	RGBColor store_color;
	Rect parts_of_area_to_draw[3] = {{0,0,17,271},{16,0,123,256},{123,0,144,271}};

	
	GetBackColor(&store_color);

	// Now place new stuff. Just draw it all there naively. It's in a gworld, and fast, so
	// who gives a shit?
	GetPort(&old_port);	
	SetPort( item_stats_gworld);

	// First clean up gworld with pretty patterns
	FillCRect(&erase_rect,bg[6]);
	if (suppress_buttons == 0)
		for (i = 0; i < 6; i++)
			FillCRect(&item_screen_button_rects[i],bg[7]);
	FillCRect(&upper_frame_rect,bg[7]);

	// Draw buttons at bottom
	if (suppress_buttons == 0) {
		for (i = 0; i < 6; i++)
			FillCRect(&item_screen_button_rects[i],bg[7]);
		}

	item_offset = GetControlValue(item_sbar);

	for (i = 0; i < 8; i++)
		for (j = 0; j < 6; j++)
			item_area_button_active[i][j] = FALSE;
	
	switch (screen_num) {
		case 6: // On special items page
			TextFace(bold);
				ForeColor(whiteColor);
			sprintf((char *) to_draw, "Special items:");
			win_draw_string( item_stats_gworld,upper_frame_rect,
 			   to_draw,0,10,false);
				ForeColor(blackColor);
			for (i = 0; i < 8; i++) {
				i_num = i + item_offset;
				if (spec_item_array[i_num] >= 0) { 
					// 2nd condition above is quite kludgy, in case it gets here with array all 0's
					strcpy((char *) to_draw,scenario.scen_strs(60 + spec_item_array[i_num] * 2));
					win_draw_string( item_stats_gworld,item_buttons[i][0],to_draw,0,10,false);
					
					place_item_button(3,i,4,0);
					if ((scenario.special_items[spec_item_array[i_num]] % 10 == 1)
					&& (!(is_combat())))
						place_item_button(0,i,3,0);
					}
				}
			break;
		case 7: // On jobs page
////
			break;
			
		default: // on an items page
			pc = screen_num; 
				ForeColor(whiteColor);
			sprintf((char *) to_draw, "%s inventory:",
				(char *) adven[pc].name);
			win_draw_string( item_stats_gworld,upper_frame_rect,
 			  to_draw,0,10,false);
				ForeColor(blackColor);
				
			for (i = 0; i < 8; i++) {
				i_num = i + item_offset;
				sprintf((char *) to_draw, "%d.",i_num + 1);
				win_draw_string( item_stats_gworld,item_buttons[i][0],
 				  to_draw,0,10,false);
 				  
 				dest_rect = item_buttons[i][0];
				dest_rect.left += 36;
				
				if (adven[pc].items[i_num].variety == 0) {
					
					}
					else {
						if (adven[pc].equip[i_num] == TRUE) {
							TextFace(italic | bold);
							if (adven[pc].items[i_num].variety < 3)
								ForeColor(magentaColor);						
								else if ((adven[pc].items[i_num].variety >= 12) && (adven[pc].items[i_num].variety <= 17))
									ForeColor(greenColor);
									else ForeColor(blueColor);
							}
							else ForeColor(blackColor);

							//// 
							if (!adven[pc].items[i_num].is_ident())
								sprintf((char *) to_draw, "%s  ",adven[pc].items[i_num].name);
								else { /// Don't place # of charges when Sell button up and space tight
									if ((adven[pc].items[i_num].charges > 0) && (adven[pc].items[i_num].type != 2)
										&& (stat_screen_mode <= 1))
										sprintf((char *) to_draw, "%s (%d)",adven[pc].items[i_num].full_name,adven[pc].items[i_num].charges);
										else sprintf((char *) to_draw, "%s",adven[pc].items[i_num].full_name);
									}
						dest_rect.left -= 2;
						win_draw_string( item_stats_gworld,dest_rect,to_draw,0,10,false);
						TextFace(0);
						TextFace(bold);
						ForeColor(blackColor);	

						// this is kludgy, awkwark, and has redundant code. Done this way to 
						// make go faster, and I got lazy.
						if ((stat_screen_mode == 0) && 
						 ((is_town()) || (is_out()) || ((is_combat()) && (pc == current_pc)))) { // place give and drop and use
							place_item_button(0,i,0,adven[pc].items[i_num].graphic_num); // item_graphic 
							if (abil_chart[adven[pc].items[i_num].ability] != 4) // place use if can
								place_item_button(10,i,1,0);
								else place_item_button(11,i,1,0);
							}
							else {
								place_item_button(0,i,0,adven[pc].items[i_num].graphic_num); // item_graphic 
								place_item_button(3,i,4,0); // info button
								if ((stat_screen_mode == 0) && 
								 ((is_town()) || (is_out()) || ((is_combat()) && (pc == current_pc)))) { // place give and drop and use
									place_item_button(1,i,2,0);
									place_item_button(2,i,3,0);
									if (abil_chart[adven[pc].items[i_num].ability] != 4) // place use if can
										place_item_button(0,i,1,0);
									}
								}
						if (stat_screen_mode > 1) {
							place_buy_button(i,pc,i_num);
						
							}
							
						} // end of if item is there
					} // end of for (i = 0; i < 8; i++)
			break;
			}
			
	place_item_bottom_buttons();	
	
	// Now put text on window.
	SetPort(GetWindowPort(mainPtr));
	BackColor(whiteColor);
	
	for (i = 0; i < 3; i++) {
		dest_rect = parts_of_area_to_draw[i];
		OffsetRect(&dest_rect,ITEM_WIN_UL_X,ITEM_WIN_UL_Y);
		rect_draw_some_item (item_stats_gworld, parts_of_area_to_draw[i], 
		  item_stats_gworld, dest_rect, 0, 1);
		}
		
	SetPort(old_port);	
	RGBBackColor(&store_color);
}

void place_buy_button(short position,short pc_num,short item_num)
{
	Rect dest_rect,source_rect;
	Rect button_sources[3] = {{24,0,36,30},{24,30,36,60},{36,0,48,30}};
	short val_to_place;
	char store_str[60];
	short aug_cost[10] = {4,7,10,8, 15,15,10, 0,0,0};
	
	if (adven[pc_num].items[item_num].variety == 0)
		return;
		
	dest_rect = item_buttons[position][5];

	val_to_place = (adven[pc_num].items[item_num].charges > 0) ?
		adven[pc_num].items[item_num].charges * adven[pc_num].items[item_num].value :
		adven[pc_num].items[item_num].value;
	val_to_place = val_to_place / 2;

	switch (stat_screen_mode) {
		case 2:
			if (!adven[pc_num].items[item_num].is_ident()) { 
				item_area_button_active[position][5] = TRUE;
				source_rect = button_sources[0];
				val_to_place = shop_identify_cost;
				}
			break;
		case 3: // sell weapons 
			if (((adven[pc_num].items[item_num].variety < 7) || (adven[pc_num].items[item_num].variety == 23) ||
				(!adven[pc_num].equip[item_num]) &&
				(adven[pc_num].items[item_num].variety == 24)) &&
				(adven[pc_num].items[item_num].is_ident()) && (val_to_place > 0) &&
				 (!adven[pc_num].items[item_num].is_cursed())) { 
				item_area_button_active[position][5] = TRUE;
				source_rect = button_sources[1];
				}
			break;
		case 4: // sell armor
			if ((adven[pc_num].items[item_num].variety >= 12) && (adven[pc_num].items[item_num].variety <= 17) &&
				(!adven[pc_num].equip[item_num]) &&
				(adven[pc_num].items[item_num].is_ident()) && (val_to_place > 0) &&
				 (!adven[pc_num].items[item_num].is_cursed())) { 
				item_area_button_active[position][5] = TRUE;
				source_rect = button_sources[1];
				}
			break;
		case 5: // sell any
			if ((val_to_place > 0) && (adven[pc_num].items[item_num].is_ident()) && 
				(!adven[pc_num].equip[item_num]) &&
				 (!adven[pc_num].items[item_num].is_cursed())) { 
				item_area_button_active[position][5] = TRUE;
				source_rect = button_sources[1];
				}
			break;
		case 6: // augment weapons 
			if ((adven[pc_num].items[item_num].variety < 3) &&
				(adven[pc_num].items[item_num].is_ident()) &&
				(adven[pc_num].items[item_num].ability == 0) &&
				(!adven[pc_num].items[item_num].is_magic())) { 
				item_area_button_active[position][5] = TRUE;
				source_rect = button_sources[2];
				val_to_place = max(aug_cost[shop_identify_cost] * 100,adven[pc_num].items[item_num].value * (5 + aug_cost[shop_identify_cost]));
				}
			break;
		}
	if (item_area_button_active[position][5]) {
		store_selling_values[position] = val_to_place;	
		dest_rect = item_buttons[position][5];
		dest_rect.right = dest_rect.left + 30;
		rect_draw_some_item (mixed_gworld, source_rect, 
		  item_stats_gworld, dest_rect, 1, 0);
		sprintf((char *) store_str,"        %d",val_to_place);
		if (val_to_place >= 10000)
			TextFace(0);
		char_port_draw_string( item_stats_gworld,item_buttons[position][5],
 		  store_str,2,10,false);	
		TextFace(bold);
		}
}

//extern Boolean item_area_button_active[8][6];
 // name, use, give, drop, info, sell/id
// shortcuts - if which_button_to_put is 10, all 4 buttons now
//				if which_button_to_put is 11, just right 2
void place_item_button(short which_button_to_put,short which_slot,short which_button_position,short extra_val)
{
	Rect from_rect = {0,0,18,18},to_rect;
	
	if (which_button_position == 0) { // this means put little item graphic, extra val is which_graphic
		item_area_button_active[which_slot][which_button_position] = TRUE;
		OffsetRect(&from_rect,(extra_val % 10) * 18,(extra_val / 10) * 18);
		to_rect = item_buttons[which_slot][0];
		to_rect.right = to_rect.left + (to_rect.bottom - to_rect.top);
		InsetRect(&to_rect,-1,-1);
		OffsetRect(&to_rect,20,1);
		InsetRect(&from_rect,2,2);
		if (extra_val >= 150) {
			from_rect = get_custom_rect(extra_val - 150);
			rect_draw_some_item (spec_scen_g, from_rect, 
			  item_stats_gworld, to_rect, 1, 0);
			}
			else rect_draw_some_item (tiny_obj_gworld, from_rect, 
			  item_stats_gworld, to_rect, 1, 0);
		 return;
		}
	if (which_button_to_put < 4) { // this means put a regular item button
		item_area_button_active[which_slot][which_button_position] = TRUE;
		rect_draw_some_item (mixed_gworld, item_buttons_from[which_button_to_put], 
		  item_stats_gworld, item_buttons[which_slot][which_button_position], 1, 0);
		}
	if (which_button_to_put == 10) { // this means put all 4
		item_area_button_active[which_slot][1] = TRUE;
		item_area_button_active[which_slot][2] = TRUE;
		item_area_button_active[which_slot][3] = TRUE;
		item_area_button_active[which_slot][4] = TRUE;
		from_rect = item_buttons_from[0]; from_rect.right = item_buttons_from[3].right;
		to_rect = item_buttons[which_slot][1];
		to_rect.right = to_rect.left + from_rect.right - from_rect.left;
		rect_draw_some_item (mixed_gworld, from_rect, 
		  item_stats_gworld, to_rect, 1, 0);
		}
	if (which_button_to_put == 11) { // this means put right 3
		item_area_button_active[which_slot][2] = TRUE;
		item_area_button_active[which_slot][3] = TRUE;
		item_area_button_active[which_slot][4] = TRUE;
		from_rect = item_buttons_from[1]; from_rect.right = item_buttons_from[3].right;
		to_rect = item_buttons[which_slot][2];
		to_rect.right = to_rect.left + from_rect.right - from_rect.left;
		rect_draw_some_item (mixed_gworld, from_rect, 
		  item_stats_gworld, to_rect, 1, 0);
		}
}
//Rect get_custom_rect (short which_rect) ////
//{
//	Rect store_rect = {0,0,36,28};
//
//	OffsetRect(&store_rect,28 * (which_rect % 10),36 * (which_rect / 10));
//	return store_rect;
//}
void place_item_bottom_buttons()
{
	Rect pc_from_rect = {0,0,36,28},but_from_rect = {85,36,101,54},to_rect;
	short i;
	
	for (i = 0; i < 6; i++) {
		if (adven[i].main_status == 1) {
		 	item_bottom_button_active[i] = TRUE;
		 	to_rect = item_screen_button_rects[i];
			rect_draw_some_item (mixed_gworld, but_from_rect, item_stats_gworld, to_rect, 0, 0);
			pc_from_rect = get_party_template_rect(i,0);
			InsetRect(&to_rect,2,2);
			rect_draw_some_item (party_template_gworld, pc_from_rect, item_stats_gworld, to_rect, 0, 0);
			}
			else item_bottom_button_active[i] = FALSE;
		}
}

Rect get_party_template_rect(short pc_num,short mode)
{
	Rect source_rect;

	source_rect.top = (pc_num % 3) * BITMAP_HEIGHT;
	source_rect.bottom = 36 + (pc_num % 3) * BITMAP_HEIGHT;
	source_rect.left = (pc_num / 3) * BITMAP_WIDTH * 2 + ((mode == 1) ? 28 : 0);
	source_rect.right = source_rect.left + BITMAP_WIDTH;
	
	return source_rect;
}


void set_stat_window(short new_stat)
{
	short i,array_pos = 0;
	
	stat_window = new_stat;
	if ((stat_window < 6) && (adven[stat_window].main_status != 1))
		stat_window = first_active_pc();
	switch (stat_window) {
		case 6:
			for (i = 0; i < 60; i++)
				spec_item_array[i] = -1;
			for (i = 0; i < 50; i++) ////
				if (party.spec_items[i] > 0) {
					spec_item_array[array_pos] = i;	
					array_pos++;
					}
			array_pos = max(0,array_pos - 8);
			SetControlMaximum(item_sbar,array_pos);
			break;
		case 7:
			SetControlMaximum(item_sbar,2);
			break;
		default:
			SetControlMaximum(item_sbar,16);
			break;
		}
	SetControlValue(item_sbar,0);
	put_item_screen(stat_window,0);

}

short first_active_pc()
{
	short i = 0;
	
	for (i = 0; i < 6; i++)
		if (adven[i].main_status == 1)
			return i;
	return 6;
}


void refresh_stat_areas(short mode)
{
	short i,x;
	Rect dest_rect,parts_of_area_to_draw[3] = {{0,0,17,271},{16,0,123,256},{123,0,144,271}};

	x = mode * 10;
	GetPortBounds(pc_stats_gworld, &dest_rect);
	Rect oldRect = dest_rect;
	OffsetRect(&dest_rect,PC_WIN_UL_X,PC_WIN_UL_Y);
	rect_draw_some_item (pc_stats_gworld, oldRect, pc_stats_gworld, dest_rect, x, 1);
	for (i = 0; i < 3; i++) {
		dest_rect = parts_of_area_to_draw[i];
		OffsetRect(&dest_rect,ITEM_WIN_UL_X,ITEM_WIN_UL_Y);
		rect_draw_some_item (item_stats_gworld, parts_of_area_to_draw[i], 
		  item_stats_gworld, dest_rect, x, 1);
		}
	GetPortBounds(text_area_gworld, &dest_rect);
	oldRect = dest_rect;
	OffsetRect(&dest_rect,TEXT_WIN_UL_X,TEXT_WIN_UL_Y);
	rect_draw_some_item (text_area_gworld,oldRect, text_area_gworld, dest_rect, x, 1);

}

////
// get job info gone

short total_encumberance(short pc_num)
{
	short store = 0,i,what_val;
	
	for (i = 0; i < 24; i++)
		if (adven[pc_num].equip[i] == TRUE) {
			what_val = adven[pc_num].items[i].awkward;
			store += what_val;
			}
	return store;
}

short get_tnl(pc_record_type *pc)
{
	short tnl = 100,i,store_per = 100;
	short rp[3] = {0,12,20};
	short ap[15] = {10,20,8,10,4, 6,10,7,12,15, -10,-8,-8,-20,-8};
	
	tnl = (tnl * (100 + rp[pc->race])) / 100;
	for (i = 0; i < 15; i++)
		if (pc->traits[i] == TRUE) 
			store_per = store_per + ap[i];

	tnl = (tnl * store_per) / 100;	
	
	return tnl;
}



void draw_pc_effects(short pc)
//short pc; // 10 + x -> draw for pc x, but on spell dialog  
{
	Rect source_rects[18] = {{55,0,67,12},{55,12,67,24},{55,24,67,36},
							{67,0,79,12},{67,12,79,24},{67,24,79,36},
							{79,0,91,12},{79,12,91,24},{79,24,91,36},
							{91,0,103,12},{91,12,103,24},{91,24,103,36},
							{103,0,115,12},{103,12,115,24},{103,24,115,36},
							{115,0,127,12},{115,12,127,24},{115,24,127,36}};
	Rect dest_rect = {18,15,30,27},dlog_dest_rect = {66,354,78,366};
	short right_limit = 250;
	short dest = 0; // 0 - in gworld  2 - on dialog
	short name_width;
	RGBColor	store_color;


	GetBackColor(&store_color);	
	BackColor(whiteColor);	
	
	if (pc >= 10) {
		pc -= 10;
		right_limit = 490;
		dest_rect = dlog_dest_rect;
		dest = 2;
		dest_rect.top += pc * 25 + 18;
		dest_rect.bottom += pc * 25 + 18;
		}
		else {
			name_width = string_length(adven[pc].name);
			right_limit = pc_buttons[0][1].left - 5;
			//dest_rect.left = pc_buttons[i][1].left - 16;
			dest_rect.left = name_width + 33;
			dest_rect.right = dest_rect.left + 12;
			dest_rect.top += pc * 13;
			dest_rect.bottom += pc * 13;
			}
			
	if (adven[pc].main_status % 10 != 1)
		return;
			
	if ((adven[pc].status[0] > 0) && (dest_rect.right < right_limit)) { 
		rect_draw_some_item (mixed_gworld,source_rects[4],pc_stats_gworld,dest_rect,1,dest);
		dest_rect.left += 13;
		dest_rect.right += 13;
		}
	if (adven[pc].status[1] > 0) { 
		rect_draw_some_item (mixed_gworld,source_rects[2],pc_stats_gworld,dest_rect,1,dest);
		dest_rect.left += 13;
		dest_rect.right += 13;
		}
	if (adven[pc].status[1] < 0) { 
		rect_draw_some_item (mixed_gworld,source_rects[3],pc_stats_gworld,dest_rect,1,dest);
		dest_rect.left += 13;
		dest_rect.right += 13;
		}
	if (adven[pc].status[2] > 0) { 
		rect_draw_some_item (mixed_gworld,source_rects[(adven[pc].status[2] > 4) ? 1 : 0],pc_stats_gworld,dest_rect,1,dest);
		dest_rect.left += 13;
		dest_rect.right += 13;
		}
	if (adven[pc].status[4] > 0) { 
		rect_draw_some_item (mixed_gworld,source_rects[5],pc_stats_gworld,dest_rect,1,dest);
		dest_rect.left += 13;
		dest_rect.right += 13;
		}
	if (adven[pc].status[3] > 0) { 
		rect_draw_some_item (mixed_gworld,source_rects[6],pc_stats_gworld,dest_rect,1,dest);
		dest_rect.left += 13;
		dest_rect.right += 13;
		}
	if (adven[pc].status[3] < 0) { 
		rect_draw_some_item (mixed_gworld,source_rects[8],pc_stats_gworld,dest_rect,1,dest);
		dest_rect.left += 13;
		dest_rect.right += 13;
		}
	if ((adven[pc].status[5] > 0) && (dest_rect.right < right_limit)) { 
		rect_draw_some_item (mixed_gworld,source_rects[9],pc_stats_gworld,dest_rect,1,dest);
		dest_rect.left += 13;
		dest_rect.right += 13;
		}
	if ((adven[pc].status[6] > 0) && (dest_rect.right < right_limit)) { 
		rect_draw_some_item (mixed_gworld,source_rects[10],pc_stats_gworld,dest_rect,1,dest);
		dest_rect.left += 13;
		dest_rect.right += 13;
		}
	if ((adven[pc].status[7] > 0) && (dest_rect.right < right_limit)){ 
		rect_draw_some_item (mixed_gworld,source_rects[11],pc_stats_gworld,dest_rect,1,dest);
		dest_rect.left += 13;
		dest_rect.right += 13;
		}
	if ((adven[pc].status[8] > 0) && (dest_rect.right < right_limit)){ 
		rect_draw_some_item (mixed_gworld,source_rects[12],pc_stats_gworld,dest_rect,1,dest);
		dest_rect.left += 13;
		dest_rect.right += 13;
		}
	if ((adven[pc].status[9] > 0) && (dest_rect.right < right_limit)){ 
		rect_draw_some_item (mixed_gworld,source_rects[13],pc_stats_gworld,dest_rect,1,dest);
		dest_rect.left += 13;
		dest_rect.right += 13;
		}	
	if ((adven[pc].status[10] > 0) && (dest_rect.right < right_limit)){ 
		rect_draw_some_item (mixed_gworld,source_rects[14],pc_stats_gworld,dest_rect,1,dest);
		dest_rect.left += 13;
		dest_rect.right += 13;
		}	
	if ((adven[pc].status[11] > 0) && (dest_rect.right < right_limit)){ 
		rect_draw_some_item (mixed_gworld,source_rects[15],pc_stats_gworld,dest_rect,1,dest);
		dest_rect.left += 13;
		dest_rect.right += 13;
		}	
	if ((adven[pc].status[12] > 0) && (dest_rect.right < right_limit)){ 
		rect_draw_some_item (mixed_gworld,source_rects[16],pc_stats_gworld,dest_rect,1,dest);
		dest_rect.left += 13;
		dest_rect.right += 13;
		}	
	if ((adven[pc].status[13] > 0) && (dest_rect.right < right_limit)){ 
		rect_draw_some_item (mixed_gworld,source_rects[17],pc_stats_gworld,dest_rect,1,dest);
		dest_rect.left += 13;
		dest_rect.right += 13;
		}	
	RGBBackColor(&store_color);
}


void print_party_stats() {
	add_string_to_buf("PARTY STATS:");
	sprintf((char *) store_string, "  Number of kills: %ld                   ", party.total_m_killed);
	add_string_to_buf((char *) store_string);
	if ((is_town()) || ((is_combat()) && (which_combat_type == 1))) {
		sprintf((char *) store_string, "  Kills in this town: %d                   ", party.m_killed[c_town.town_num]);
		add_string_to_buf((char *) store_string);
		}
	sprintf((char *) store_string, "  Total experience: %ld                   ", party.total_xp_gained);
	add_string_to_buf((char *) store_string);
	sprintf((char *) store_string, "  Total damage done: %ld                   ", party.total_dam_done);
	add_string_to_buf((char *) store_string);
	sprintf((char *) store_string, "  Total damage taken: %ld                   ", party.total_dam_taken);
	add_string_to_buf((char *) store_string);
	print_buf();
}


short do_look(location space)
{
	short i,j,num_items = 0;
	Boolean gold_here = FALSE, food_here = FALSE, is_lit = TRUE;
	location from_where;
		
	from_where = get_cur_loc();
	is_lit = pt_in_light(from_where,space);

	if (((overall_mode == MODE_LOOK_OUTDOORS) && (space == party.p_loc)) ||
		((overall_mode == MODE_LOOK_TOWN) && (space == c_town.p_loc)))
			add_string_to_buf("    Your party");
	if (overall_mode == MODE_LOOK_COMBAT)
		for (i = 0; i < 6; i++)
			if ((space == pc_pos[i]) && (adven[i].main_status == 1)
				&& (is_lit == TRUE) && (can_see(pc_pos[current_pc],space,0) < 5)) {
				sprintf((char *) store_string, "    %s", (char *) adven[i].name);
				add_string_to_buf((char *) store_string);					
				}
		
	if ((overall_mode == MODE_LOOK_TOWN) || (overall_mode == MODE_LOOK_COMBAT)) {
		for (i = 0; i < T_M; i++)
			if ((c_town.monst.dudes[i].active != 0) && (is_lit == TRUE)
				&& (monst_on_space(space,i) == TRUE) &&
				((overall_mode == MODE_LOOK_TOWN) || (can_see(pc_pos[current_pc],space,0) < 5))
				&& (c_town.monst.dudes[i].m_d.picture_num != 0)) {
				
				
				get_m_name(store_string2,c_town.monst.dudes[i].number);
				if (c_town.monst.dudes[i].m_d.health < c_town.monst.dudes[i].m_d.m_health) {
					if (c_town.monst.dudes[i].attitude % 2 == 1)
						sprintf((char *) store_string, "    Wounded %s (H)", store_string2);
						else sprintf((char *) store_string, "    Wounded %s (F)", store_string2);
					}
				else {
					if (c_town.monst.dudes[i].attitude % 2 == 1)
						sprintf((char *) store_string, "    %s (H)", (char *) store_string2);
						else sprintf((char *) store_string, "    %s (F)", (char *) store_string2);
					}
				
				add_string_to_buf((char *) store_string);					

				}
		}
	if (overall_mode == MODE_LOOK_OUTDOORS) {
		for (i = 0; i < 10; i++) {
			if ((party.out_c[i].exists) 
				&& (space == party.out_c[i].m_loc)) {
					for (j = 0; j < 7; j++) 
						if (party.out_c[i].what_monst.monst[j] != 0) {
							get_m_name(store_string2,party.out_c[i].what_monst.monst[j]);
							sprintf((char *) store_string, "    %s", store_string2);
							add_string_to_buf((char *) store_string);		
							j = 7;
						
							}
				
					}
			}
	
		if (out_boat_there(space) < 30)
			add_string_to_buf("    Boat                ");
		if (out_horse_there(space) < 30)
			add_string_to_buf("    Horse                ");
		}
		
	if ((overall_mode == MODE_LOOK_TOWN) || (overall_mode == MODE_LOOK_COMBAT)) {
		if (town_boat_there(space) < 30)
			add_string_to_buf("    Boat               ");
		if (town_horse_there(space) < 30)
			add_string_to_buf("    Horse               ");

		if (is_web(space.x,space.y))
			add_string_to_buf("    Web               ");
		if (is_crate(space.x,space.y))
			add_string_to_buf("    Crate               ");
		if (is_barrel(space.x,space.y))
			add_string_to_buf("    Barrel               ");
		if (is_fire_barrier(space.x,space.y))
			add_string_to_buf("    Magic Barrier               ");
		if (is_force_barrier(space.x,space.y))
			add_string_to_buf("    Magic Barrier               ");
		if (is_quickfire(space.x,space.y))
			add_string_to_buf("    Quickfire               ");
		if (is_fire_wall(space.x,space.y))
			add_string_to_buf("    Wall of Fire               ");
		if (is_force_wall(space.x,space.y))
			add_string_to_buf("    Wall of Force               ");
		if (is_antimagic(space.x,space.y))
			add_string_to_buf("    Antimagic Field               ");
		if (is_scloud(space.x,space.y))
			add_string_to_buf("    Stinking Cloud               ");
		if (is_ice_wall(space.x,space.y))
			add_string_to_buf("    Ice Wall               ");
		if (is_blade_wall(space.x,space.y))
			add_string_to_buf("    Blade Wall               ");

		if (is_small_blood(space.x,space.y))
			add_string_to_buf("    Blood stain               ");
		if (is_medium_blood(space.x,space.y))
			add_string_to_buf("    Blood stain               ");
		if (is_large_blood(space.x,space.y))
			add_string_to_buf("    Blood stain               ");
		if (is_small_slime(space.x,space.y))
			add_string_to_buf("    Smears of slime               ");
		if (is_big_slime(space.x,space.y))
			add_string_to_buf("    Smears of slime               ");
		if (is_ash(space.x,space.y))
			add_string_to_buf("    Ashes               ");
		if (is_bones(space.x,space.y))
			add_string_to_buf("    Bones               ");
		if (is_rubble(space.x,space.y))
			add_string_to_buf("    Rubble               ");
		
		for (i = 0; i < NUM_TOWN_ITEMS; i++) {
			if ((t_i.items[i].variety != 0) && (space == t_i.items[i].item_loc)
				&& (is_lit == TRUE)) {
				if (t_i.items[i].variety == 3)
					gold_here = TRUE;
				else if (t_i.items[i].variety == 11)
					food_here = TRUE;
					else num_items++;
				}
			}
		if (gold_here == TRUE)
			add_string_to_buf("    Gold");
		if (food_here == TRUE)
			add_string_to_buf("    Food");
		if (num_items > 8)
			add_string_to_buf("    Many items");
			else for (i = 0; i < NUM_TOWN_ITEMS; i++) {
				if ((t_i.items[i].variety != 0) && (t_i.items[i].variety != 3) &&(t_i.items[i].variety != 11) &&
				    (space == t_i.items[i].item_loc) && (!t_i.items[i].is_contained())) {		
					if (t_i.items[i].is_ident())
						sprintf((char *) store_string, "    %s",t_i.items[i].full_name); 
						else sprintf((char *) store_string, "    %s",t_i.items[i].name);				
					add_string_to_buf((char *) store_string);
					}
				}
		}
		
	if (is_lit == FALSE) {
		add_string_to_buf("    Dark                ");
		return 0;
		}
			
	return print_terrain(space);
}

short town_boat_there(location where)
{
	short i;
	
	// Num boats stores highest # of boat in town
	for (i = 0; i < 30; i++)
		if ((party.boats[i].exists) && (party.boats[i].which_town == c_town.town_num) 
		 && (where == party.boats[i].loc))
			return i;
	return 30;
}
short out_boat_there(location where)
{
	short i;
	
	for (i = 0; i < 30; i++)
		if ((party.boats[i].exists) && (where == party.boats[i].loc)
			&& (party.boats[i].which_town == 200))
			return i;
	return 30;
}

short town_horse_there(location where)
{
	short i;
	
	// Num boats stores highest # of boat in town
	for (i = 0; i < 30; i++)
		if ((party.horses[i].exists) && (party.horses[i].which_town == c_town.town_num) 
		 && (where == party.horses[i].loc))
			return i;
	return 30;
}
short out_horse_there(location where)
{
	short i;
	
	for (i = 0; i < 30; i++)
		if ((party.horses[i].exists) && (where == party.horses[i].loc)
			&& (party.horses[i].which_town == 200))
			return i;
	return 30;
}
void notify_out_combat_began(out_wandering_type encounter,short *nums) 
{
	short i;

	sprintf((char *) store_string, "COMBAT!                 ");				
	add_string_to_buf((char *) store_string);	

	for (i = 0; i < 6; i++)
		if (encounter.monst[i] != 0) {
			switch (encounter.monst[i]) {
				////
				
				default:
				get_m_name(store_string2,encounter.monst[i]);
				sprintf((char *) store_string, "  %d x %s        ",nums[i],store_string2);
				break;		
			}				
			add_string_to_buf((char *) store_string);	
			}
	if (encounter.monst[6] != 0) {
			get_m_name(store_string2,encounter.monst[6]);
			sprintf((char *) store_string, "  %s        ",store_string2);
			add_string_to_buf((char *) store_string);		
		}
}

void get_m_name(char *str,unsigned char num)
{
	
	////
	strcpy((char *) str,(char *) scenario.scen_monsters[num].m_name);
}
void get_ter_name(char *str,unsigned char num)
{
	Str255 store_name;
	
	////
	if ((num == 90) && ((is_out()) || (is_town()) || ((is_combat()) && (which_combat_type == 1))))
		sprintf((char *) store_name,"Pit");
		else {
			strcpy((char *) store_name,(char *) scenario.ter_names[num]);
			}
	strcpy((char *) str,(char *) store_name);
}

void print_monst_name(unsigned char m_type)
{
	get_m_name(store_string2,m_type);
	sprintf ((char *) store_string, "%s:",store_string2);
	add_string_to_buf((char *) store_string);
}

void print_monst_attacks(unsigned char m_type,short target)
//short target; // < 100 - pc  >= 100  monst
{
	char store_string3[60];
	
	get_m_name(store_string2,m_type);
	if (target < 100)
		sprintf ((char *) store_string, "%s attacks %s",
			store_string2,(char *) adven[target].name);
		else {
			get_m_name(store_string3,c_town.monst.dudes[target - 100].number);
			sprintf ((char *) store_string, "%s attacks %s",
			store_string2,store_string3);
			}
	add_string_to_buf((char *) store_string);
}

////
void damaged_message(short damage,short type)
{
	Str255 str;
	
	get_str(str,20,130 + type);
	sprintf ((char *) store_string, "  %s for %d",
			(char *) str,damage);
	add_string_to_buf((char *) store_string);	
}

// This prepares the monster's string for the text bar
void print_monster_going(char *combat_str,unsigned char m_num,short ap)
{
	get_m_name(store_string2,m_num);
	sprintf ((char *) combat_str, "%s (ap: %d)",
		store_string2,ap);
}

void monst_spell_note(unsigned char number,short which_mess)
{
	get_m_name(store_string2,number);
	switch (which_mess) {
		case 1:
	sprintf ((char *) store_string, "  %s scared. ",store_string2);break;
	
		case 2:
	sprintf ((char *) store_string, "  %s slowed. ",store_string2);break;
	
		case 3:
	sprintf ((char *) store_string, "  %s weakened.",store_string2);break;
	
		case 4:
	sprintf ((char *) store_string, "  %s poisoned.",store_string2);break;
	
		case 5:
	sprintf ((char *) store_string, "  %s cursed.",store_string2);break;

		case 6:
	sprintf ((char *) store_string, "  %s ravaged.",store_string2);break;

		case 7:
	sprintf ((char *) store_string, "  %s undamaged.",store_string2);break;

		case 8:
	sprintf ((char *) store_string, "  %s is stoned.",store_string2);break;
		case 9:
	sprintf ((char *) store_string, "  Gazes at %s.",store_string2);break;
		case 10:
	sprintf ((char *) store_string, "  %s resists.",store_string2);break;		
		case 11:
	sprintf ((char *) store_string, "  Drains %s.",store_string2);break;	
		case 12:
	sprintf ((char *) store_string, "  Shoots at %s.",store_string2);break;	
		case 13:
	sprintf ((char *) store_string, "  Throws spear at %s.",
		store_string2);
			break;	
		case 14:
	sprintf ((char *) store_string, "  Throws rock at %s.",
		store_string2);
			break;	
		case 15:
	sprintf ((char *) store_string, "  Throws razordisk at %s.",
		store_string2);
			break;
		case 16:
	sprintf ((char *) store_string, "  Hits %s.",
		store_string2);
			break;
		case 17:
	sprintf ((char *) store_string, "%s disappears.",
		store_string2);
			break;
		case 18:
	sprintf ((char *) store_string, "  Misses %s.",
		store_string2);
			break;
		case 19:
	sprintf ((char *) store_string, "  %s is webbed.",store_string2);break;
		case 20:
	sprintf ((char *) store_string, "  %s chokes.",store_string2);break;
		case 21:
	sprintf ((char *) store_string, "  %s summoned.",store_string2);break;
		case 22:
	sprintf ((char *) store_string, "  %s is dumbfounded.",store_string2);break;
		case 23:
	sprintf ((char *) store_string, "  %s is charmed.",store_string2);break;
		case 24:
	sprintf ((char *) store_string, "  %s is recorded.",store_string2);break;
		case 25:
	sprintf ((char *) store_string, "  %s is diseased.",store_string2);break;
		case 26:
	sprintf ((char *) store_string, "  %s is an avatar!",store_string2);break;
		case 27:
	sprintf ((char *) store_string, "  %s splits!",store_string2);break;
		case 28:
	sprintf ((char *) store_string, "  %s falls asleep.",store_string2);break;
		case 29:
	sprintf ((char *) store_string, "  %s wakes up.",store_string2);break;
		case 30:
	sprintf ((char *) store_string, "  %s paralyzed.",store_string2);break;
		case 31:
	sprintf ((char *) store_string, "  %s covered with acid.",store_string2);break;
		case 32:
	sprintf ((char *) store_string, "  Fires spines at %s.",store_string2);break;
		case 33:
	sprintf ((char *) store_string, "  %s summons aid.",store_string2);break;
		}

	if (which_mess > 0)
		add_string_to_buf((char *) store_string);
}

void monst_cast_spell_note(unsigned char number,short spell,short type)
//short type; // 0 - mage 1- priest
{
	get_m_name(store_string2,number);
	sprintf ((char *) store_string, "%s casts:",
			(char *) store_string2);
	add_string_to_buf((char *) store_string);
	sprintf ((char *) store_string, "  %s",
			(type == 1) ? (char *) m_priest_sp[spell - 1] : (char *) m_mage_sp[spell - 1]);
	add_string_to_buf((char *) store_string);
}

void monst_breathe_note(unsigned char number)
{
	get_m_name(store_string2,number);
	sprintf ((char *) store_string, "%s breathes.",
			(char *) store_string2);
	add_string_to_buf((char *) store_string);

}

void monst_damaged_mes(short which_m,short how_much,short how_much_spec)
{
	get_m_name(store_string2,c_town.monst.dudes[which_m].number);
	if (how_much_spec > 0)
		sprintf ((char *) store_string, "  %s takes %d+%d",
			store_string2, how_much,how_much_spec);
		else sprintf ((char *) store_string, "  %s takes %d",
			store_string2, how_much);
 
	add_string_to_buf((char *) store_string);
}

void monst_killed_mes(short which_m)
{
	get_m_name(store_string2,c_town.monst.dudes[which_m].number);
	sprintf ((char *) store_string, "  %s dies.",
		(char *) store_string2);
	add_string_to_buf((char *) store_string);
}

void print_nums(short a,short b,short c)
{
	sprintf((char *) store_string, "debug: %d %d %d", a,b,c);
	add_string_to_buf((char *) store_string);

}

short print_terrain(location space)
{
	unsigned char which_terrain;

	if (overall_mode == MODE_LOOK_OUTDOORS) {
		which_terrain = out[space.x][space.y];
		}
	if (overall_mode == MODE_LOOK_TOWN) {
		which_terrain = t_d.terrain[space.x][space.y];
		}
	if (overall_mode == MODE_LOOK_COMBAT) {
		which_terrain = combat_terrain[space.x][space.y];
		}
	get_ter_name(store_string2,which_terrain);
	sprintf((char *) store_string, "    %s", store_string2);
	add_string_to_buf((char *) store_string);

	return (short) which_terrain;
}


void add_string_to_buf(char *string)
{
	if (in_startup_mode == TRUE)
		return;
		
	SetControlValue(text_sbar,58);
	if (buf_pointer == mark_where_printing_long) {
		printing_long = TRUE;
		print_buf();
		through_sending();
		}
	sprintf((char *)text_buffer[buf_pointer].line, "%-49.49s", string);
//	c2pstr((char *)text_buffer[buf_pointer].line);
	if (buf_pointer == (TEXT_BUF_LEN - 1))
		buf_pointer = 0;
		else buf_pointer++;

}

void init_buf()
{
	short i;
	
	for (i = 0; i < TEXT_BUF_LEN; i++)
		sprintf((char *) text_buffer[buf_pointer].line, " ");
}

void print_buf () 
{
	short num_lines_printed = 0,ctrl_val;
	short line_to_print;
	short start_print_point;
	GrafPtr old_port;
	Boolean end_loop = FALSE;
	Rect store_text_rect,dest_rect,erase_rect = {1,1,137,255};

	// Now place new stuff. Just draw it all there naively. It's in a gworld, and fast, so
	// who gives a shit?
	GetPort(&old_port);	
	SetPort( text_area_gworld);

	// First clean up gworld with pretty patterns
	InsetRect(&erase_rect,1,1); ////
	erase_rect.right++;
	FillCRect(&erase_rect,bg[6]);

	ctrl_val = 58 - GetControlValue(text_sbar);
	start_print_point = buf_pointer - LINES_IN_TEXT_WIN - ctrl_val;
	if (start_print_point< 0)
		start_print_point= TEXT_BUF_LEN + start_print_point;
	line_to_print= start_print_point;
	
	while ((line_to_print!= buf_pointer) && (num_lines_printed < LINES_IN_TEXT_WIN)) {
		MoveTo(4, 13 + 12 * num_lines_printed);
		drawstring((char *) text_buffer[line_to_print].line);
		num_lines_printed++;
		line_to_print++;
		if (line_to_print== TEXT_BUF_LEN) {
			line_to_print= 0;
			}
			
		if ((num_lines_printed == LINES_IN_TEXT_WIN - 1) && (printing_long == TRUE)) {
			end_loop = FALSE;
			line_to_print= buf_pointer;
			}
	
		}
		
	
	GetPortBounds(text_area_gworld, &store_text_rect);
	dest_rect = store_text_rect;

	OffsetRect(&dest_rect,TEXT_WIN_UL_X,TEXT_WIN_UL_Y);
	SetPort(GetWindowPort(mainPtr));
	rect_draw_some_item (text_area_gworld, store_text_rect, text_area_gworld, dest_rect, 0, 1);
	SetPort(old_port);		
}

void restart_printing()
{
	lines_to_print = 0;
	//clear_text_panel();
}

void restore_mode()
{
	overall_mode = store_mode;
}

void through_sending()
{
	mark_where_printing_long = buf_pointer + LINES_IN_TEXT_WIN - 1;
	if (mark_where_printing_long > TEXT_BUF_LEN - 1)
		mark_where_printing_long -= TEXT_BUF_LEN;
	printing_long = FALSE;
}

void Display_String(Str255 str)
{
	Str255 str2;
	
	//c2pstr((char *) str);
	sprintf((char *)str2," %s",str);
	str2[0] = (char) strlen((char *)str);
	DrawString(str2);
}

void drawstring(char * str)
{
	unsigned char pstr[256];
	c2pstrcpy(pstr,str);
	DrawString(pstr);
}

void display_string(char *str)
{
//	c2pstr(str);
	drawstring(str);
}


/* Draw a bitmap in the world window. hor in 0 .. 8, vert in 0 .. 8,
	object is ptr. to bitmap to be drawn, and masking is for Copybits. */
void Draw_Some_Item (GWorldPtr src_gworld, Rect src_rect, GWorldPtr targ_gworld, 
location target, char masked, short main_win)
{
Rect	destrec = {0,0,36,28};
PixMapHandle	test1, test2;
const BitMap * store_dest;
	
	if ((target.x < 0) || (target.y < 0) || (target.x > 8) || (target.y > 8))
		return;
	if (src_gworld == NULL)
		return;
	if ((supressing_some_spaces == TRUE) && (target != ok_space[0]) &&
		(target != ok_space[1]) && (target != ok_space[2]) && (target != ok_space[3]))
			return;
	terrain_there[target.x][target.y] = -1;
	
	store_dest = GetPortBitMapForCopyBits(GetWindowPort(mainPtr));	
	
	test1 = GetPortPixMap(src_gworld);

	destrec = coord_to_rect(target.x,target.y);
	if (main_win == 1) { 
		if (cartoon_happening == TRUE) {
			OffsetRect(&destrec,ul.h,ul.v);
			if (store_anim_type == 0) 
				OffsetRect(&destrec,306,5);
				else OffsetRect(&destrec,store_anim_ul.h,store_anim_ul.v);
			}
			else OffsetRect(&destrec,ul.h + 5,ul.v + 5);
		}
		
	LockPixels(test1);
	if (main_win == 0) {
		test2 = GetPortPixMap(targ_gworld); 
		LockPixels(test2);
		if (masked == 1) 
			CopyBits ( (BitMap *) *test1 ,
					(BitMap *) *test2 ,
					&src_rect, &destrec, 
					 transparent , NULL);	
			else CopyBits ( (BitMap *) *test1 ,
					(BitMap *) *test2 ,
					&src_rect, &destrec, 
					  srcCopy, NULL);
		UnlockPixels(test2);
		}  
		else {

		if (masked == 1) 
			CopyBits ( (BitMap *) *test1 ,
					store_dest ,
					&src_rect, &destrec, 
					 transparent , NULL);
			else CopyBits ( (BitMap *) *test1 ,
					store_dest ,
					&src_rect, &destrec, 
					  srcCopy , NULL);   
			}
	UnlockPixels(test1);
}


//void rect_draw_some_item (GWorldPtr src_gworld, Rect src_rect,  GWorldPtr targ_gworld, 
//Rect targ_rect,  char masked, short main_win)
////	 masked; // if 10 - make AddOver
////   main_win; // if 2, drawing onto dialog
//{
//	PixMapHandle	test1, test2;
//	const BitMap * store_dest;
//	GrafPtr cur_port;
//	RGBColor	store_color;
//
//	GetPort(&cur_port);	
//	if (src_gworld == NULL) {
//		if (main_win == 0) {
//			SetPort ( targ_gworld);
//			PaintRect(&targ_rect);
//			SetPort (cur_port);
//			}
//			else PaintRect(&targ_rect);
//		return;
//		}
//	if (main_win == 2) {
//		GetBackColor(&store_color);
//		BackColor(whiteColor);
//		}
//		
//	store_dest = GetPortBitMapForCopyBits(cur_port);	
//	
//	test1 =  GetPortPixMap(src_gworld);
//
//	if (main_win == 1) 	
//		OffsetRect(&targ_rect,ul.h,ul.v);
//
//	LockPixels(test1);
//	if (main_win == 0) {
//		test2 =  GetPortPixMap(targ_gworld); 
//		LockPixels(test2);
//		if (masked == 1) 
//			CopyBits ( (BitMap *) *test1 ,
//					(BitMap *) *test2 ,
//					&src_rect, &targ_rect, 
//					 transparent , NULL);	
//			else CopyBits ( (BitMap *) *test1 ,
//					(BitMap *) *test2 ,
//					&src_rect, &targ_rect, 
//				  (masked == 10) ? addOver : 0, NULL);
//		UnlockPixels(test2);
//		}  
//		else {
//		if (masked == 1) 
//			CopyBits ( (BitMap *) *test1 ,
//					store_dest ,
//					&src_rect, &targ_rect, 
//					 transparent , NULL);
//			else CopyBits ( (BitMap *) *test1 ,
//					store_dest ,
//					&src_rect, &targ_rect, 
//					  (masked == 10) ? addOver : 0, NULL);
//			}
//	UnlockPixels(test1);
//	if (main_win == 2) 
//		RGBBackColor(&store_color);
//	SetPort(cur_port);
//}

Rect coord_to_rect(short i,short j)
{
	Rect to_return;
	
	to_return.left = 13 + BITMAP_WIDTH * i;
	to_return.right = to_return.left + BITMAP_WIDTH;
	to_return.top = 13 + BITMAP_HEIGHT * j;
	to_return.bottom = to_return.top + BITMAP_HEIGHT;		

	return to_return;
}


void make_cursor_sword() 
{
	set_cursor(sword_curs);
}

//short string_length(char *str)
//{
//	short text_len[257];
//	short total_width = 0,i,len;
//	Str255 p_str;
//	
//	for (i = 0; i < 257; i++)
//		text_len[i]= 0;
//	
//	strcpy((char *) p_str,str);
//	c2pstr(p_str);
//	MeasureText(256,p_str,text_len);
//	len = strlen((char *)str);
//	
//	for (i = 0; i < 257; i++)
//		if ((text_len[i] > total_width) && (i <= len))
//			total_width = text_len[i];
//	return total_width;
//}


//void char_win_draw_string(WindowPtr dest_window,Rect dest_rect,char *str,short mode,short line_height)
//{
//	char_port_draw_string(GetWindowPort(dest_window), dest_rect, str, mode, line_height); 
//}
//
//
//void char_port_draw_string(GrafPtr dest_window,Rect dest_rect,char *str,short mode,short line_height)
//{
//	Str255 store_s;
//	
//	strcpy((char *) store_s,str);
//	win_draw_string( dest_window, dest_rect,store_s, mode, line_height);
//
//}

// mode: 0 - align up and left, 1 - center on one line
// str is a c string, 256 characters
// uses current font
//void win_draw_string(GrafPtr dest_window,Rect dest_rect,Str255 str,short mode,short line_height)
//{
//	GrafPtr old_port;
//	Str255 p_str,str_to_draw,str_to_draw2,c_str;
//	Str255 null_s = "                                                                                                                                                                                                                                                              ";
//	short str_len,i;
//	short last_line_break = 0,last_word_break = 0,on_what_line = 0;
//	short text_len[257];
//	short total_width = 0;
//	Boolean force_skip = FALSE;
//	RgnHandle current_clip;
//	short adjust_x = 0,adjust_y = 0;
//	
//	if (dest_window ==  GetWindowPort(mainPtr)) {
//		adjust_x = ul.h; adjust_y = ul.v;
//		}
//	strcpy((char *) p_str,(char *) str);
//	strcpy((char *) c_str,(char *) str);
//	c2pstr(p_str);	
//	for (i = 0; i < 257; i++)
//		text_len[i]= 0;
//	MeasureText(256,p_str,text_len);
//	str_len = (short) strlen((char *)str);
//	if (str_len == 0) {
//		return;
//		}
//
//	GetPort(&old_port);	
//	SetPort(dest_window);
//	
//	//FrameRect(&dest_rect);
//	
//	current_clip = NewRgn();
//	GetClip(current_clip);
//	
//	dest_rect.bottom += 5;
//	//ClipRect(&dest_rect);
//	dest_rect.bottom -= 5;
//	
//	
//	for (i = 0; i < 257; i++)
//		if ((text_len[i] > total_width) && (i <= str_len))
//			total_width = text_len[i];
//	if ((mode == 0) && (total_width < dest_rect.right - dest_rect.left))
//		mode = 2;
//	for (i = 0; i < 257; i++)
//		if ((i <= str_len) && (c_str[i] == '|') && (mode == 2))
//			mode = 0;
//		
//
//	switch (mode) {
//		case 0: 
//			MoveTo(dest_rect.left + 1 + adjust_x, dest_rect.top + 1 + line_height * on_what_line + adjust_y + 9);
//			for (i = 0;text_len[i] != text_len[i + 1], i < str_len;i++) {
//				if (((text_len[i] - text_len[last_line_break] > (dest_rect.right - dest_rect.left - 6)) 
//				  && (last_word_break > last_line_break)) || (c_str[i] == '|')) {
//				  	if (c_str[i] == '|') {
//				  		c_str[i] = ' ';
//				  		force_skip = TRUE;
//				  		last_word_break = i + 1;
//				  		}
//					sprintf((char *)str_to_draw,"%s",(char *)null_s);
//					strncpy ((char *) str_to_draw,(char *) c_str + last_line_break,(size_t) (last_word_break - last_line_break - 1));
//					sprintf((char *)str_to_draw2," %s",str_to_draw);
//					str_to_draw2[0] = (char) strlen((char *)str_to_draw);
//					DrawString(str_to_draw2);
//					on_what_line++;
//					MoveTo(dest_rect.left + 1 + adjust_x, dest_rect.top + 1 + line_height * on_what_line + adjust_y + 9);
//					last_line_break = last_word_break;
//					if (force_skip == TRUE) {
//						force_skip = FALSE;
//						i++;
//						//last_line_break++;
//						//last_word_break++;
//						}
//					}
//				if (c_str[i] == ' ')
//					last_word_break = i + 1;
//				if (on_what_line == LINES_IN_TEXT_WIN - 1)
//					i = 10000;
//				}
//	
//			if (i - last_line_break > 1) {
//				strcpy((char *)str_to_draw,(char *)null_s);
//				strncpy ((char *) str_to_draw,(char *) c_str + last_line_break,(size_t) (i - last_line_break));
//				sprintf((char *)str_to_draw2," %s",str_to_draw);
//				if (strlen((char *) str_to_draw2) > 3) {
//					str_to_draw2[0] = (char) strlen((char *)str_to_draw);
//					DrawString(str_to_draw2);
//					}
//				}	
//			break;
//		case 1:
//			MoveTo((dest_rect.right + dest_rect.left) / 2 - (4 * total_width) / 9 + adjust_x, 
//			  (dest_rect.bottom + dest_rect.top - line_height) / 2 + 9 + adjust_y);	
//			DrawText(p_str,1,p_str[0]);
//			//DrawString(p_str);
//			break;
//		case 2:
//			MoveTo(dest_rect.left + 1 + adjust_x, 
//			  dest_rect.top + 1 + adjust_y + 9);
//			DrawString(p_str);					
//			break;
//		case 3:
//			MoveTo(dest_rect.left + 1 + adjust_x, 
//			  dest_rect.top + 1 + adjust_y + 9 + (dest_rect.bottom - dest_rect.top) / 6);
//			DrawString(p_str);					
//			break;
//		}
//	SetClip(current_clip);
//	DisposeRgn(current_clip);
//	SetPort(old_port);
//}

short calc_day()
{
	return (short) ((party.age) / 3700) + 1;
}

Boolean day_reached(unsigned char which_day, unsigned char which_event)
// which_day is day event should happen
// which_event is the party.key_times value to cross reference with. 
// if the key_time is reached before which_day, event won't happen
// if it's 0, event always happens
{
	if (which_event > 10)
		return FALSE;
	if ((which_event > 0) && (party.key_times[which_event] < which_day))
		return FALSE;
	if (calc_day() >= which_day)
		return TRUE;
		else return FALSE;
}