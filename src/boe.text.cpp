#define  LINES_IN_TEXT_WIN	11
#define	TEXT_BUF_LEN	70

#include <sstream>

#include "boe.global.h"

#include "universe.h"
#include "boe.text.h"
#include "boe.locutils.h"
#include "mathutil.hpp"
#include "graphtool.hpp"
#include "scrollbar.hpp"
#include "restypes.hpp"
#include "spell.hpp"

typedef struct {
	char line[50];
} buf_line;

buf_line text_buffer[TEXT_BUF_LEN];
short buf_pointer = 30, lines_to_print= 0, num_added_since_stop = 0;
short start_print_point= 0;
short mark_where_printing_long;
bool printing_long = false;
char c_str[256] = "";
bool save_mess_given = false;

rectangle status_panel_clip_rect = {11, 299, 175, 495},item_panel_clip_rect = {11,297,175,463};

// TODO: The duplication of rectangle here shouldn't be necessary...
rectangle item_buttons_from[7] = {
	rectangle{11,0,24,14},rectangle{11,14,24,28},rectangle{11,28,24,42},rectangle{11,42,24,56},
	rectangle{24,0,36,30},rectangle{24,30,36,60},rectangle{36,0,48,30}};

eGameMode store_mode;

extern short had_text_freeze;
extern eStatMode stat_screen_mode;

// graphics globals
extern rectangle status_panel_rect,status_panel_title_rect;
extern rectangle	text_panel_rect;
extern short which_combat_type,stat_window;
extern eGameMode overall_mode;
extern sf::RenderWindow mainPtr;
extern rectangle more_info_button;
extern short which_item_page[6];
extern std::shared_ptr<cScrollbar> text_sbar,item_sbar;
extern location store_anim_ul;
extern tessel_ref_t bg[];
extern short dest_personalities[40];
extern location source_locs[6];
extern location dest_locs[40] ;

extern sf::Texture tiny_obj_gworld,invenbtn_gworld,status_gworld;
extern cCustomGraphics spec_scen_g;
extern sf::Texture pc_gworld;
extern sf::RenderTexture pc_stats_gworld, item_stats_gworld, text_area_gworld;
extern short terrain_there[9][9];

// game globals
extern location ul;
extern bool play_sounds;
extern rectangle item_buttons[8][6];
// name, use, give, drip, info, sell/id
extern rectangle pc_buttons[6][5];
// name, hp, sp, info, trade
extern bool item_area_button_active[8][6];
extern bool pc_area_button_active[6][5];
extern rectangle item_screen_button_rects[9];
extern short spec_item_array[60];
extern std::map<eItemAbil, short> abil_chart;
// combat globals
extern short item_bottom_button_active[9];
extern cUniverse univ;
extern short current_pc;
extern short shop_identify_cost;
extern short store_selling_values[8];
extern short combat_posing_monster, current_working_monster; // 0-5 PC 100 + x - monster x
extern bool supressing_some_spaces;
extern location ok_space[4];
extern sf::Texture bg_gworld;
extern std::map<eStatus, std::pair<short, short>> statIcons;

// Draws the pc area in upper right
void put_pc_screen() {
	short i = 0,j;
	rectangle erase_rect = {17,2,98,269},to_draw_rect,from_rect;
	// TODO: The duplication of rectangle here shouldn't be necessary...
	rectangle small_erase_rects[3] = {rectangle{101,34,114,76},rectangle{101,106,114,147},rectangle{101,174,114,201}};
	rectangle bottom_bar_rect = {99,0,116,271};
	rectangle info_from = {0,1,12,13};
	
	pc_stats_gworld.setActive();
	
	// First clean up gworld with pretty patterns
	tileImage(pc_stats_gworld, erase_rect,bg[6]);
	for(i = 0; i < 3; i++)
		tileImage(pc_stats_gworld, small_erase_rects[i],bg[7]);
	
	TextStyle style;
	style.font = FONT_BOLD;
	style.pointSize = 12;
	style.colour = sf::Color::White;
	style.lineHeight = 10;
	// Put food, gold, day
	win_draw_string(pc_stats_gworld,small_erase_rects[1],std::to_string(univ.party.gold),eTextMode::WRAP,style);
	win_draw_string(pc_stats_gworld,small_erase_rects[0],std::to_string(univ.party.food),eTextMode::WRAP,style);
	win_draw_string(pc_stats_gworld,small_erase_rects[2],std::to_string(calc_day()),eTextMode::WRAP,style);
	style.colour = sf::Color::Black;
	
	for(i = 0; i < 6; i++) {
		if(univ.party[i].main_status != eMainStatus::ABSENT) {
			for(j = 0; j < 5; j++)
				pc_area_button_active[i][j] = 1;
			if(i == current_pc) {
				style.italic = true;
				style.colour = sf::Color::Blue;
			}
			
			std::ostringstream sout;
			sout << i + 1 << ". " << univ.party[i].name;
			win_draw_string(pc_stats_gworld,pc_buttons[i][0],sout.str(),eTextMode::WRAP,style);
			style.italic = false;
			style.colour = sf::Color::Black;
			
			to_draw_rect = pc_buttons[i][1];
			to_draw_rect.right += 20;
			sout.str("");
			switch(univ.party[i].main_status) {
				case eMainStatus::ALIVE:
					if(univ.party[i].cur_health == univ.party[i].max_health)
						style.colour = sf::Color::Green;
					else if(univ.party[i].cur_health > univ.party[i].max_health)
						style.colour = {0xff,0x80,0}; // Orange
					else style.colour = sf::Color::Red;
					win_draw_string( pc_stats_gworld,pc_buttons[i][1],std::to_string(univ.party[i].cur_health),eTextMode::WRAP,style);
					if(univ.party[i].cur_sp == univ.party[i].max_sp)
						style.colour = sf::Color::Blue;
					else if(univ.party[i].cur_sp > univ.party[i].max_sp)
						style.colour = {0,0xff,0x80}; // Teal
					else style.colour = sf::Color::Magenta;
					win_draw_string( pc_stats_gworld,pc_buttons[i][2],std::to_string(univ.party[i].cur_sp),eTextMode::WRAP,style);
					draw_pc_effects(i);
					break;
				case eMainStatus::DEAD:
					sout << "Dead";
					break;
				case eMainStatus::DUST:
					sout << "Dust";
					break;
				case eMainStatus::STONE:
					sout << "Stone";
					break;
				case eMainStatus::FLED:
					sout << "Fled";
					break;
				case eMainStatus::SURFACE:
					sout << "Surface";
					break;
				case eMainStatus::WON:
					sout << "Won";
					break;
				default:
					sout << "Absent";
					break;
			}
			if(univ.party[i].main_status != eMainStatus::ALIVE)
				win_draw_string( pc_stats_gworld,to_draw_rect,sout.str(),eTextMode::WRAP,style);
			style.colour = sf::Color::Black;
			
			// Now put trade and info buttons
			//rect_draw_some_item(mixed_gworld,info_from,pc_stats_gworld,pc_buttons[i][3],1,0);
			//rect_draw_some_item(mixed_gworld,switch_from,pc_stats_gworld,pc_buttons[i][4],1,0);
			// do faster!
			to_draw_rect = pc_buttons[i][3];
			to_draw_rect.right = pc_buttons[i][4].right + 1;
			from_rect = info_from;
			from_rect.right = from_rect.left + to_draw_rect.right - to_draw_rect.left;
			rect_draw_some_item(invenbtn_gworld,from_rect,pc_stats_gworld,to_draw_rect,sf::BlendAlpha);
		}
		else {
			for(j = 0; j < 5; j++)
				pc_area_button_active[i][j] = 0;
		}
	}
	pc_stats_gworld.display();
	
	// Sometimes this gets called when character is slain. when that happens, if items for
	// that PC are up, switch item page.
	if(current_pc < 6 && univ.party[current_pc].main_status != eMainStatus::ALIVE && stat_window == current_pc) {
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
// if suppress_buttons > 0, save time by not redrawing buttons
void put_item_screen(short screen_num,short suppress_buttons) {
	std::ostringstream sout;
	long i_num;
	long item_offset;
	short i = 0,j,pc;
	rectangle erase_rect = {17,2,122,255},dest_rect;
	rectangle upper_frame_rect = {3,3,15,268};
	
	item_stats_gworld.setActive();
	
	// First clean up gworld with pretty patterns
	tileImage(item_stats_gworld, erase_rect,bg[6]);
	if(suppress_buttons == 0)
		for(i = 0; i < 6; i++)
			tileImage(item_stats_gworld, item_screen_button_rects[i],bg[7]);
	tileImage(item_stats_gworld, upper_frame_rect,bg[7]);
	
	// Draw buttons at bottom
	if(suppress_buttons == 0) {
		for(i = 0; i < 6; i++)
			tileImage(item_stats_gworld, item_screen_button_rects[i],bg[7]);
	}
	
	item_offset = item_sbar->getPosition();
	
	for(i = 0; i < 8; i++)
		for(j = 0; j < 6; j++)
			item_area_button_active[i][j] = false;
	
	TextStyle style;
	style.lineHeight = 10;
	switch(screen_num) {
		case ITEM_WIN_SPECIAL:
			style.font = FONT_BOLD;
			style.colour = sf::Color::White;
			win_draw_string(item_stats_gworld,upper_frame_rect,"Special items:",eTextMode::WRAP,style);
			style.colour = sf::Color::Black;
			for(i = 0; i < 8; i++) {
				i_num = i + item_offset;
				if(spec_item_array[i_num] >= 0) {
					// 2nd condition above is quite kludgy, in case it gets here with array all 0's
					win_draw_string(item_stats_gworld,item_buttons[i][0],univ.scenario.special_items[spec_item_array[i_num]].name,eTextMode::WRAP,style);
					
					place_item_button(3,i,4,0);
					if((univ.scenario.special_items[spec_item_array[i_num]].flags % 10 == 1)
						&& (!(is_combat())))
						place_item_button(0,i,3,0);
				}
			}
			break;
		case ITEM_WIN_QUESTS:
			// TODO: Implement quest list
			break;
			
		default: // on an items page
			pc = screen_num;
			style.colour = sf::Color::White;
			style.font = FONT_PLAIN;
			sout.str("");;
			sout << univ.party[pc].name << " inventory:",
			win_draw_string(item_stats_gworld,upper_frame_rect,sout.str(),eTextMode::WRAP,style);
			style.colour = sf::Color::Black;
			style.font = FONT_BOLD;
			
			for(i = 0; i < 8; i++) {
				i_num = i + item_offset;
				sout.str("");
				sout << i_num + 1 << '.';
				win_draw_string(item_stats_gworld,item_buttons[i][0],sout.str(),eTextMode::WRAP,style);
				
 				dest_rect = item_buttons[i][0];
				dest_rect.left += 36;
				
				if(univ.party[pc].items[i_num].variety == eItemType::NO_ITEM) {
					
				}
				else {
					style.font = FONT_PLAIN;
					if(univ.party[pc].equip[i_num]) {
						style.italic = true;
						if(univ.party[pc].items[i_num].variety == eItemType::ONE_HANDED || univ.party[pc].items[i_num].variety == eItemType::TWO_HANDED)
							style.colour = sf::Color::Magenta;
						else if(isArmourType(univ.party[pc].items[i_num].variety))
							style.colour = sf::Color::Green;
						else style.colour = sf::Color::Blue;
					} else style.colour = sf::Color::Black;
					
					sout.str("");
					if(!univ.party[pc].items[i_num].ident)
						sout << univ.party[pc].items[i_num].name << "  ";
					else { /// Don't place # of charges when Sell button up and space tight
						sout << univ.party[pc].items[i_num].full_name << ' ';
						// TODO: Why are bashing weapons excluded from this?
						if(univ.party[pc].items[i_num].charges > 0 && univ.party[pc].items[i_num].weap_type != eSkill::BASHING_WEAPONS
						   && (stat_screen_mode == MODE_INVEN || stat_screen_mode == MODE_SHOP))
							sout << '(' << int(univ.party[pc].items[i_num].charges) << ')';
					}
					dest_rect.left -= 2;
					win_draw_string(item_stats_gworld,dest_rect,sout.str(),eTextMode::WRAP,style);
					style.italic = false;
					style.colour = sf::Color::Black;
					
					// this is kludgy, awkwark, and has redundant code. Done this way to
					// make go faster, and I got lazy.
					if((stat_screen_mode == MODE_SHOP) &&
						((is_town()) || (is_out()) || ((is_combat()) && (pc == current_pc)))) { // place give and drop and use
						place_item_button(0,i,0,univ.party[pc].items[i_num].graphic_num); // item_graphic
						if(abil_chart[univ.party[pc].items[i_num].ability]) // place use if can
							place_item_button(10,i,1,0);
						else place_item_button(11,i,1,0);
					}
					else {
						place_item_button(0,i,0,univ.party[pc].items[i_num].graphic_num); // item_graphic
						place_item_button(3,i,4,0); // info button
						if((stat_screen_mode == MODE_INVEN) &&
							((is_town()) || (is_out()) || ((is_combat()) && (pc == current_pc)))) { // place give and drop and use
							place_item_button(1,i,2,0);
							place_item_button(2,i,3,0);
							if(abil_chart[univ.party[pc].items[i_num].ability]) // place use if can
								place_item_button(0,i,1,0);
						}
					}
					if(stat_screen_mode != MODE_INVEN && stat_screen_mode != MODE_SHOP) {
						place_buy_button(i,pc,i_num);
						
					}
					
				} // end of if item is there
			} // end of for(i = 0; i < 8; i++)
			break;
	}
	
	place_item_bottom_buttons();
	item_stats_gworld.display();
}

void place_buy_button(short position,short pc_num,short item_num) {
	rectangle dest_rect,source_rect;
	// TODO: The duplication of rectangle here shouldn't be necessary...
	rectangle button_sources[3] = {rectangle{24,0,36,30},rectangle{36,0,48,30},rectangle{48,0,60,30}};
	short val_to_place;
	// TODO: This is now duplicated here and in start_town_mode()
	short aug_cost[10] = {4,7,10,8, 15,15,10, 0,0,0};
	
	if(univ.party[pc_num].items[item_num].variety == eItemType::NO_ITEM)
		return;
	
	dest_rect = item_buttons[position][5];
	
	val_to_place = (univ.party[pc_num].items[item_num].charges > 0) ?
	univ.party[pc_num].items[item_num].charges * univ.party[pc_num].items[item_num].value :
	univ.party[pc_num].items[item_num].value;
	val_to_place = val_to_place / 2;
	
	switch(stat_screen_mode) {
		case MODE_IDENTIFY:
			if(!univ.party[pc_num].items[item_num].ident) {
				item_area_button_active[position][5] = true;
				source_rect = button_sources[0];
				val_to_place = shop_identify_cost;
			}
			break;
		case MODE_SELL_WEAP:
			if(isWeaponType(univ.party[pc_num].items[item_num].variety) &&
				(!univ.party[pc_num].equip[item_num]) &&
				(univ.party[pc_num].items[item_num].ident) && (val_to_place > 0) &&
				(!univ.party[pc_num].items[item_num].unsellable)) {
				item_area_button_active[position][5] = true;
				source_rect = button_sources[1];
			}
			break;
		case MODE_SELL_ARMOR:
			if(isArmourType(univ.party[pc_num].items[item_num].variety) &&
			   (!univ.party[pc_num].equip[item_num]) &&
			   (univ.party[pc_num].items[item_num].ident) && (val_to_place > 0) &&
			   (!univ.party[pc_num].items[item_num].unsellable)) {
				item_area_button_active[position][5] = true;
				source_rect = button_sources[1];
			}
			break;
		case MODE_SELL_ANY:
			if((val_to_place > 0) && (univ.party[pc_num].items[item_num].ident) &&
				(!univ.party[pc_num].equip[item_num]) &&
				(!univ.party[pc_num].items[item_num].unsellable)) {
				item_area_button_active[position][5] = true;
				source_rect = button_sources[1];
			}
			break;
		case MODE_ENCHANT:
			if((univ.party[pc_num].items[item_num].variety == eItemType::ONE_HANDED || univ.party[pc_num].items[item_num].variety == eItemType::TWO_HANDED) &&
				(univ.party[pc_num].items[item_num].ident) &&
				univ.party[pc_num].items[item_num].ability == eItemAbil::NONE &&
				(!univ.party[pc_num].items[item_num].magic)) {
				item_area_button_active[position][5] = true;
				source_rect = button_sources[2];
				val_to_place = max(aug_cost[shop_identify_cost] * 100,univ.party[pc_num].items[item_num].value * (5 + aug_cost[shop_identify_cost]));
			}
			break;
		case MODE_INVEN: case MODE_SHOP:
			// These modes don't have buy buttons, so we shouldn't get here; bail out!
			return;
	}
	if(item_area_button_active[position][5]) {
		store_selling_values[position] = val_to_place;
		dest_rect = item_buttons[position][5];
		dest_rect.right = dest_rect.left + 30;
		rect_draw_some_item(invenbtn_gworld, source_rect, item_stats_gworld, dest_rect, sf::BlendAlpha);
		TextStyle style;
		if(val_to_place >= 10000)
			style.font = FONT_PLAIN;
		style.lineHeight = 10;
		dest_rect.offset(dest_rect.width() + 5,0);
		win_draw_string(item_stats_gworld,dest_rect,std::to_string(val_to_place),eTextMode::LEFT_TOP,style);
	}
}

// name, use, give, drop, info, sell/id
// shortcuts - if which_button_to_put is 10, all 4 buttons now
//				if which_button_to_put is 11, just right 2
void place_item_button(short which_button_to_put,short which_slot,short which_button_position,short extra_val) {
	rectangle from_rect = {0,0,18,18},to_rect;
	
	if(which_button_position == 0) { // this means put little item graphic, extra val is which_graphic
		item_area_button_active[which_slot][which_button_position] = true;
		from_rect.offset((extra_val % 10) * 18,(extra_val / 10) * 18);
		to_rect = item_buttons[which_slot][0];
		to_rect.right = to_rect.left + (to_rect.bottom - to_rect.top);
		to_rect.inset(-1,-1);
		to_rect.offset(20,1);
		from_rect.inset(2,2);
		if(extra_val >= 10000) {
			sf::Texture* src_gw;
			graf_pos_ref(src_gw, from_rect) = spec_scen_g.find_graphic(extra_val - 10000, true);
			rect_draw_some_item(*src_gw, from_rect, item_stats_gworld, to_rect,sf::BlendAlpha);
		} else if(extra_val >= 1000) {
			sf::Texture* src_gw;
			graf_pos_ref(src_gw, from_rect) = spec_scen_g.find_graphic(extra_val - 1000);
			rect_draw_some_item(*src_gw, from_rect, item_stats_gworld, to_rect,sf::BlendAlpha);
		}
		else rect_draw_some_item(tiny_obj_gworld, from_rect, item_stats_gworld, to_rect, sf::BlendAlpha);
		return;
	}
	if(which_button_to_put < 4) { // this means put a regular item button
		item_area_button_active[which_slot][which_button_position] = true;
		rect_draw_some_item(invenbtn_gworld, item_buttons_from[which_button_to_put], item_stats_gworld, item_buttons[which_slot][which_button_position], sf::BlendAlpha);
	}
	if(which_button_to_put == 10) { // this means put all 4
		item_area_button_active[which_slot][1] = true;
		item_area_button_active[which_slot][2] = true;
		item_area_button_active[which_slot][3] = true;
		item_area_button_active[which_slot][4] = true;
		from_rect = item_buttons_from[0]; from_rect.right = item_buttons_from[3].right;
		to_rect = item_buttons[which_slot][1];
		to_rect.right = to_rect.left + from_rect.right - from_rect.left;
		rect_draw_some_item(invenbtn_gworld, from_rect, item_stats_gworld, to_rect, sf::BlendAlpha);
	}
	if(which_button_to_put == 11) { // this means put right 3
		item_area_button_active[which_slot][2] = true;
		item_area_button_active[which_slot][3] = true;
		item_area_button_active[which_slot][4] = true;
		from_rect = item_buttons_from[1]; from_rect.right = item_buttons_from[3].right;
		to_rect = item_buttons[which_slot][2];
		to_rect.right = to_rect.left + from_rect.right - from_rect.left;
		rect_draw_some_item(invenbtn_gworld, from_rect, item_stats_gworld, to_rect, sf::BlendAlpha);
	}
}

void place_item_bottom_buttons() {
	rectangle pc_from_rect = {0,0,36,28},but_from_rect = {30,60,46,78},to_rect;
	rectangle spec_from_rect = {0,60,15,95}, job_from_rect = {15,60,30,95}, help_from_rect = {46,60,59,76};
	// TODO: What about when the buttons are pressed?
	short i;
	
	for(i = 0; i < 6; i++) {
		if(univ.party[i].main_status == eMainStatus::ALIVE) {
		 	item_bottom_button_active[i] = true;
		 	to_rect = item_screen_button_rects[i];
			rect_draw_some_item(invenbtn_gworld, but_from_rect, item_stats_gworld, to_rect, sf::BlendAlpha);
			pc_from_rect = calc_rect(2 * (univ.party[i].which_graphic / 8), univ.party[i].which_graphic % 8);
			to_rect.inset(2,2);
			rect_draw_some_item(pc_gworld, pc_from_rect, item_stats_gworld, to_rect, sf::BlendAlpha);
		}
		else item_bottom_button_active[i] = false;
	}
	to_rect = item_screen_button_rects[6];
	rect_draw_some_item(invenbtn_gworld, spec_from_rect, item_stats_gworld, to_rect, sf::BlendAlpha);
	to_rect = item_screen_button_rects[7];
	rect_draw_some_item(invenbtn_gworld, job_from_rect, item_stats_gworld, to_rect, sf::BlendAlpha);
	to_rect = item_screen_button_rects[8];
	rect_draw_some_item(invenbtn_gworld, help_from_rect, item_stats_gworld, to_rect, sf::BlendAlpha);
}

void set_stat_window(short new_stat) {
	short i,array_pos = 0;
	
	stat_window = new_stat;
	if(stat_window < ITEM_WIN_SPECIAL && univ.party[stat_window].main_status != eMainStatus::ALIVE)
		stat_window = first_active_pc();
	item_sbar->setPageSize(8);
	switch(stat_window) {
		case ITEM_WIN_SPECIAL:
			for(i = 0; i < 60; i++)
				spec_item_array[i] = -1;
			for(i = 0; i < 50; i++) ////
				if(univ.party.spec_items[i]) {
					spec_item_array[array_pos] = i;
					array_pos++;
				}
			array_pos = max(0,array_pos - 8);
			item_sbar->setMaximum(array_pos);
			break;
		case ITEM_WIN_QUESTS:
			item_sbar->setMaximum(2);
			item_sbar->setPageSize(2);
			break;
		default:
			item_sbar->setMaximum(16);
			break;
	}
	item_sbar->setPosition(0);
	put_item_screen(stat_window,0);
	
}

short first_active_pc() {
	short i = 0;
	
	for(i = 0; i < 6; i++)
		if(univ.party[i].main_status == eMainStatus::ALIVE)
			return i;
	return 6;
}


void refresh_stat_areas(short mode) {
	short i;
	sf::BlendMode x;
	// TODO: The duplication of rectangle here shouldn't be necessary...
	rectangle dest_rect,parts_of_area_to_draw[3] = {rectangle{0,0,17,271},rectangle{16,0,123,256},rectangle{123,0,144,271}};
	
	dest_rect = rectangle(pc_stats_gworld);
	rectangle oldRect = dest_rect;
	dest_rect.offset(PC_WIN_UL_X,PC_WIN_UL_Y);
	if(mode == 1) x = sf::BlendAdd;
	else x = sf::BlendNone;
	rect_draw_some_item (pc_stats_gworld.getTexture(), oldRect, dest_rect,ul, x);
	for(i = 0; i < 3; i++) {
		dest_rect = parts_of_area_to_draw[i];
		dest_rect.offset(ITEM_WIN_UL_X,ITEM_WIN_UL_Y);
		rect_draw_some_item(item_stats_gworld.getTexture(), parts_of_area_to_draw[i], dest_rect,ul, x);
	}
	dest_rect = rectangle(text_area_gworld);
	oldRect = dest_rect;
	dest_rect.offset(TEXT_WIN_UL_X,TEXT_WIN_UL_Y);
	rect_draw_some_item(text_area_gworld.getTexture(),oldRect, dest_rect,ul, x);
}

// get job info gone

short total_encumberance(short pc_num) {
	short store = 0,i,what_val;
	
	for(i = 0; i < 24; i++)
		if(univ.party[pc_num].equip[i]) {
			what_val = univ.party[pc_num].items[i].awkward;
			store += what_val;
		}
	return store;
}

rectangle get_stat_effect_rect(int code) {
	rectangle base = {0,0,12,12};
	base.offset(12 * (code % 3), 12 * (code / 3));
	return base;
}

void draw_pc_effects(short pc) {
	rectangle dest_rect = {18,15,30,27},dlog_dest_rect = {66,354,78,366};
	short right_limit = 250;
	short name_width;
	
	TextStyle style;
	name_width = string_length(univ.party[pc].name, style);
	right_limit = pc_buttons[0][1].left - 5;
	dest_rect.left = name_width + 33;
	dest_rect.right = dest_rect.left + 12;
	dest_rect.top += pc * 13;
	dest_rect.bottom += pc * 13;
	
	if(exceptSplit(univ.party[pc].main_status) != eMainStatus::ALIVE)
		return;
	
	univ.party[pc].status[eStatus::HASTE_SLOW]; // This just makes sure it exists in the map, without changing its value if it does
	for(auto next : univ.party[pc].status) {
		short placedIcon = -1;
		if(next.first == eStatus::POISON && next.second > 4) placedIcon = 1;
		else if(next.second > 0) placedIcon = statIcons[next.first].first;
		else if(next.second < 0) placedIcon = statIcons[next.first].second;
		else if(next.first == eStatus::HASTE_SLOW) placedIcon = 7;
		if(placedIcon >= 0) {
			rect_draw_some_item(status_gworld, get_stat_effect_rect(placedIcon), pc_stats_gworld, dest_rect, sf::BlendAlpha);
			dest_rect.offset(13, 0);
		}
		if(dest_rect.right >= right_limit) break;
	}
}


void print_party_stats() {
	add_string_to_buf("PARTY STATS:");
	add_string_to_buf("  Number of kills: " + std::to_string(univ.party.total_m_killed));
	if((is_town()) || ((is_combat()) && (which_combat_type == 1))) {
		add_string_to_buf("  Kills in this town: " + std::to_string(univ.party.m_killed[univ.town.num]));
	}
	add_string_to_buf("  Total experience: " + std::to_string(univ.party.total_xp_gained));
	add_string_to_buf("  Total damage done: " + std::to_string(univ.party.total_dam_done));
	add_string_to_buf("  Total damage taken: " + std::to_string(univ.party.total_dam_taken));
	print_buf();
}


short do_look(location space) {
	short i,j,num_items = 0;
	bool gold_here = false, food_here = false, is_lit = true;
	location from_where;
	std::string msg;
	
	from_where = get_cur_loc();
	is_lit = is_out() || pt_in_light(from_where,space);
	
	if(((overall_mode == MODE_LOOK_OUTDOORS) && (space == univ.party.p_loc)) ||
		((overall_mode == MODE_LOOK_TOWN) && (space == univ.town.p_loc)))
		add_string_to_buf("    Your party");
	if(overall_mode == MODE_LOOK_COMBAT)
		for(i = 0; i < 6; i++)
			if(space == univ.party[i].combat_pos && univ.party[i].main_status == eMainStatus::ALIVE
			   && (is_lit) && (can_see_light(univ.party[current_pc].combat_pos,space,sight_obscurity) < 5)) {
				msg = "    " + univ.party[i].name;
				add_string_to_buf(msg);
			}
	
	if((overall_mode == MODE_LOOK_TOWN) || (overall_mode == MODE_LOOK_COMBAT)) {
		for(i = 0; i < univ.town.monst.size(); i++)
			if((univ.town.monst[i].active != 0) && (is_lit)
				&& univ.town.monst[i].on_space(space) &&
				((overall_mode == MODE_LOOK_TOWN) || (can_see_light(univ.party[current_pc].combat_pos,space,sight_obscurity) < 5))
				&& (univ.town.monst[i].picture_num != 0)) {
				
				
				msg = get_m_name(univ.town.monst[i].number);
				if(univ.town.monst[i].health < univ.town.monst[i].m_health) {
					if(univ.town.monst[i].attitude % 2 == 1)
						msg = "    Wounded " + msg + " (H)";
					else msg = "    Wounded " + msg + " (F)";
				}
				else {
					if(univ.town.monst[i].attitude % 2 == 1)
						msg = "    " + msg + " (H)";
					else msg = "    " + msg + " (F)";
				}
				
				add_string_to_buf((char *) msg.c_str());
				
			}
	}
	if(overall_mode == MODE_LOOK_OUTDOORS) {
		for(i = 0; i < 10; i++) {
			if((univ.party.out_c[i].exists)
				&& (space == univ.party.out_c[i].m_loc)) {
				for(j = 0; j < 7; j++)
					if(univ.party.out_c[i].what_monst.monst[j] != 0) {
						msg = get_m_name(univ.party.out_c[i].what_monst.monst[j]);
						msg = "    " + msg;
						add_string_to_buf((char *) msg.c_str());
						j = 7;
						
					}
				
			}
		}
		
		if(out_boat_there(space) < 30)
			add_string_to_buf("    Boat");
		if(out_horse_there(space) < 30)
			add_string_to_buf("    Horse");
	}
	
	if((overall_mode == MODE_LOOK_TOWN) || (overall_mode == MODE_LOOK_COMBAT)) {
		if(town_boat_there(space) < 30)
			add_string_to_buf("    Boat");
		if(town_horse_there(space) < 30)
			add_string_to_buf("    Horse");
		
		if(univ.town.is_web(space.x,space.y))
			add_string_to_buf("    Web");
		if(univ.town.is_crate(space.x,space.y))
			add_string_to_buf("    Crate");
		if(univ.town.is_barrel(space.x,space.y))
			add_string_to_buf("    Barrel");
		if(univ.town.is_block(space.x,space.y))
			add_string_to_buf("    Stone Block");
		if(univ.town.is_fire_barr(space.x,space.y))
			add_string_to_buf("    Magic Barrier");
		if(univ.town.is_force_barr(space.x,space.y))
			add_string_to_buf("    Magic Barrier");
		if(univ.town.is_quickfire(space.x,space.y))
			add_string_to_buf("    Quickfire");
		if(univ.town.is_fire_wall(space.x,space.y))
			add_string_to_buf("    Wall of Fire");
		if(univ.town.is_force_wall(space.x,space.y))
			add_string_to_buf("    Wall of Force");
		if(univ.town.is_antimagic(space.x,space.y))
			add_string_to_buf("    Antimagic Field");
		if(univ.town.is_scloud(space.x,space.y))
			add_string_to_buf("    Stinking Cloud");
		if(univ.town.is_ice_wall(space.x,space.y))
			add_string_to_buf("    Ice Wall");
		if(univ.town.is_blade_wall(space.x,space.y))
			add_string_to_buf("    Blade Wall");
		if(univ.town.is_force_cage(space.x,space.y))
			add_string_to_buf("    Force Cage");
		
		if(univ.town.is_sm_blood(space.x,space.y))
			add_string_to_buf("    Blood stain");
		if(univ.town.is_med_blood(space.x,space.y))
			add_string_to_buf("    Blood stain");
		if(univ.town.is_lg_blood(space.x,space.y))
			add_string_to_buf("    Blood stain");
		if(univ.town.is_sm_slime(space.x,space.y))
			add_string_to_buf("    Smears of slime");
		if(univ.town.is_lg_slime(space.x,space.y))
			add_string_to_buf("    Smears of slime");
		if(univ.town.is_ash(space.x,space.y))
			add_string_to_buf("    Ashes");
		if(univ.town.is_bones(space.x,space.y))
			add_string_to_buf("    Bones");
		if(univ.town.is_rubble(space.x,space.y))
			add_string_to_buf("    Rubble");
		
		for(i = 0; i < univ.town.items.size(); i++) {
			if(univ.town.items[i].variety != eItemType::NO_ITEM && space == univ.town.items[i].item_loc
			   && (is_lit)) {
				if(univ.town.items[i].variety == eItemType::GOLD)
					gold_here = true;
				else if(univ.town.items[i].variety == eItemType::FOOD)
					food_here = true;
				else num_items++;
			}
		}
		if(gold_here)
			add_string_to_buf("    Gold");
		if(food_here)
			add_string_to_buf("    Food");
		if(num_items > 8)
			add_string_to_buf("    Many items");
		else for(i = 0; i < univ.town.items.size(); i++) {
			if(univ.town.items[i].variety != eItemType::NO_ITEM && univ.town.items[i].variety != eItemType::GOLD && univ.town.items[i].variety != eItemType::FOOD &&
			   (space == univ.town.items[i].item_loc) && (!univ.town.items[i].contained)) {
				if(univ.town.items[i].ident)
					msg = "    " + univ.town.items[i].full_name;
				else msg = "    " + univ.town.items[i].name;
				add_string_to_buf((char *) msg.c_str());
			}
		}
	}
	
	if(!is_lit) {
		add_string_to_buf("    Dark");
		return 0;
	}
	
	return print_terrain(space);
}

short town_boat_there(location where) {
	short i;
	
	// Num boats stores highest # of boat in town
	for(i = 0; i < 30; i++)
		if((univ.party.boats[i].exists) && (univ.party.boats[i].which_town == univ.town.num)
			&& (where == univ.party.boats[i].loc))
			return i;
	return 30;
}
short out_boat_there(location where) {
	short i;
	
	for(i = 0; i < 30; i++)
		if((univ.party.boats[i].exists) && (where == univ.party.boats[i].loc)
			&& (univ.party.boats[i].which_town == 200))
			return i;
	return 30;
}

short town_horse_there(location where) {
	short i;
	
	// Num boats stores highest # of boat in town
	for(i = 0; i < 30; i++)
		if((univ.party.horses[i].exists) && (univ.party.horses[i].which_town == univ.town.num)
			&& (where == univ.party.horses[i].loc))
			return i;
	return 30;
}
short out_horse_there(location where) {
	short i;
	
	for(i = 0; i < 30; i++)
		if((univ.party.horses[i].exists) && (where == univ.party.horses[i].loc)
			&& (univ.party.horses[i].which_town == 200))
			return i;
	return 30;
}
void notify_out_combat_began(cOutdoors::cWandering encounter,short *nums) {
	short i;
	std::string msg;
	
	add_string_to_buf("COMBAT!");
	
	for(i = 0; i < 6; i++)
		if(encounter.monst[i] != 0) {
			msg = get_m_name(encounter.monst[i]);
			std::ostringstream sout;
			sout << "  " << nums[i] << " x " << msg;
			msg = sout.str();
			add_string_to_buf(msg);
		}
	if(encounter.monst[6] != 0) {
		msg = "  " +  get_m_name(encounter.monst[6]);
		add_string_to_buf(msg);
	}
}

std::string get_m_name(mon_num_t num) {
	if(num >= 10000) return univ.party.summons[num - 10000].m_name;
	return univ.scenario.scen_monsters[num].m_name;
}
std::string get_ter_name(ter_num_t num) {
	std::string store_name = "Pit";
	
	if((num == 90) && ((is_out()) || (is_town()) || ((is_combat()) && (which_combat_type == 1))));
	else {
		store_name = univ.scenario.ter_types[num].name;
	}
	return store_name;
}

void print_monst_name(mon_num_t m_type) {
	std::string msg = get_m_name(m_type) + ':';
	add_string_to_buf((char *) msg.c_str());
}

void cCreature::print_attacks(iLiving* target) {
	std::string msg = m_name;
	msg += " attacks ";
	if(cPlayer* who = dynamic_cast<cPlayer*>(target))
		msg += who->name;
	else if(cCreature* monst = dynamic_cast<cCreature*>(target))
		msg += monst->m_name;
	else msg += "you";
	add_string_to_buf(msg);
}

void damaged_message(short damage,eMonstMelee type) {
	std::ostringstream sout;
	sout << "    " << get_str("monster-abilities",130 + int(type));
	sout << " for " << damage;
	add_string_to_buf(sout.str().c_str());
}

// This prepares the monster's string for the text bar
std::string print_monster_going(mon_num_t m_num,short ap) {
	std::ostringstream sout;
	sout << get_m_name(m_num);
	sout << " (ap: " << ap << ')';
	return sout.str();
}

void cCreature::spell_note(int which_mess) {
	std::string msg = m_name;
	switch(which_mess) {
		case 1:
			msg = "  " + msg + " scared.";
			break;
			
		case 2:
			msg = "  " + msg + " slowed.";
			break;
			
		case 3:
			msg = "  " + msg + " weakened.";
			break;
			
		case 4:
			msg = "  " + msg + " poisoned.";
			break;
			
		case 5:
			msg = "  " + msg + " cursed.";
			break;
			
		case 6:
			msg = "  " + msg + " ravaged.";
			break;
			
		case 7:
			msg = "  " + msg + " undamaged.";
			break;
			
		case 8:
			msg = "  " + msg + " is stoned.";
			break;
		case 9:
			msg = "  Gazes at " + msg + '.';
			break;
		case 10:
			msg = "  " + msg + " resists.";
			break;
		case 11:
			msg = "  Drains " + msg + '.';
			break;
		case 12:
			msg = "  Shoots at " + msg + '.';
			break;
		case 13:
			msg = "  Throws spear at " + msg + '.';
			break;
		case 14:
			msg = "  Throws rock at " + msg + '.';
			break;
		case 15:
			msg = "  Throws razordisk at " + msg + '.';
			break;
		case 16:
			msg = "  Hits " + msg + '.';
			break;
		case 17:
			msg = "  " + msg + " disappears.";
			break;
		case 18:
			msg = "  Misses " + msg + '.';
			break;
		case 19:
			msg = "  " + msg + " is webbed.";
			break;
		case 20:
			msg = "  " + msg + " chokes.";
			break;
		case 21:
			msg = "  " + msg + " summoned.";
			break;
		case 22:
			msg = "  " + msg + " is dumbfounded.";
			break;
		case 23:
			msg = "  " + msg + " is charmed.";
			break;
		case 24:
			msg = "  " + msg + " is recorded.";
			break;
		case 25:
			msg = "  " + msg + " is diseased.";
			break;
		case 26:
			msg = "  " + msg + " is an avatar!";
			break;
		case 27:
			msg = "  " + msg + " splits!";
			break;
		case 28:
			msg = "  " + msg + " falls asleep.";
			break;
		case 29:
			msg = "  " + msg + " wakes up.";
			break;
		case 30:
			msg = "  " + msg + " paralyzed.";
			break;
		case 31:
			msg = "  " + msg + " covered with acid.";
			break;
		case 32:
			msg = "  Fires spines at " + msg + '.';
			break;
		case 33:
			msg = "  " + msg + " summons aid.";
			break;
		case 34:
			msg = "  " + msg + " is cured.";
			break;
		case 35:
			msg = "  " + msg + " is hasted.";
			break;
		case 36:
			msg = "  " + msg + " is blessed.";
			break;
		case 37:
			msg = "  " + msg + " cleans webs.";
			break;
		case 38:
			msg = "  " + msg + " feels better.";
			break;
		case 39:
			msg = "  " + msg + " mind cleared.";
			break;
		case 40:
			msg = "  " + msg + " feels alert.";
			break;
		case 41:
			msg = "  " + msg + " is healed.";
			break;
		case 42:
			msg = "  " + msg + " drained of health.";
			break;
		case 43:
			msg = "  " + msg + " magic recharged.";
			break;
		case 44:
			msg = "  " + msg + " drained of magic.";
			break;
		case 45:
			msg = "  " + msg + " returns to life!";
			break;
		case 46:
			msg = "  " + msg + " dies.";
			break;
		case 47:
			msg = "  " + msg + " rallies its courage.";
			break;
		case 48:
			msg = "  " + msg + " cleans off acid.";
			break;
		case 49:
			msg = "  " + msg + " breaks barrier.";
			break;
		case 50:
			msg = "  " + msg + " breaks force cage.";
			break;
		case 51:
			msg = "  " + msg + " is obliterated!";
			break;
		case 52:
			msg = "  " + msg + " is trapped!";
			break;
		case 53:
			msg = "  Throws dart at " + msg + '.';
			break;
		case 54:
			msg = "  Throws knife at " + msg + '.';
			break;
		case 55:
			msg = "  Fires ray at " + msg + '.';
			break;
		case 56:
			msg = "  Gazes at " + msg + '.';
			break;
		case 57:
			msg = "  Breathes on " + msg + '.';
			break;
		case 58:
			msg = "  Throws web at " + msg + '.';
			break;
		case 59:
			msg = "  Spits at " + msg + '.';
			break;
		default:
			msg += ": Unknown action " + std::to_string(which_mess);
	}
	
	if(which_mess > 0)
		add_string_to_buf((char *) msg.c_str());
}

void cCreature::cast_spell_note(eSpell spell) {
	add_string_to_buf(m_name + " casts:");
	add_string_to_buf("  " + (*spell).name());
}

void cCreature::breathe_note() {
	add_string_to_buf(m_name + " breathes.");
}

void cCreature::damaged_msg(int how_much,int how_much_spec) {
	std::ostringstream sout;
	sout << "  " << m_name << " takes " << how_much;
	if(how_much_spec > 0)
		sout << '+' << how_much_spec;
	add_string_to_buf(sout.str());
}

void cCreature::killed_msg() {
	add_string_to_buf("  " + m_name + " dies.");
}

void print_nums(short a,short b,short c) {
	std::ostringstream sout;
	sout << "debug: " << a << ' ' << b << ' ' << c;
	add_string_to_buf((char *) sout.str().c_str());
	
}

short print_terrain(location space) {
	ter_num_t which_terrain;
	
	if(overall_mode == MODE_LOOK_OUTDOORS) {
		which_terrain = univ.out[space.x][space.y];
	}
	if(overall_mode == MODE_LOOK_TOWN || overall_mode == MODE_LOOK_COMBAT)
		which_terrain = univ.town->terrain(space.x,space.y);
	std::string msg = get_ter_name(which_terrain);
	msg = "    " + msg;
	add_string_to_buf(msg);
	
	return (short) which_terrain;
}

void add_string_to_buf(std::string str) {
	// This is a separate overload instead of using a defaulted parameter so that
	// it can be passed as an argument to various other functions
	add_string_to_buf(str, 0);
}

void add_string_to_buf(std::string str, unsigned short indent) {
	static bool inited;
	static size_t width;
	static TextStyle buf_style;
	if(!inited) {
		inited = true;
		buf_style.font = FONT_PLAIN;
		buf_style.pointSize = 12;
		width = text_area_gworld.getSize().x - 5;
	}
	if(overall_mode == MODE_STARTUP)
		return;
	
	if(str == "") return;
	if(str.find_last_not_of(' ') == std::string::npos)
		return;
	
	if(indent && string_length(str.substr(0,str.find_last_not_of(' ')), buf_style) >= width) {
		if(indent > 20) indent = 20;
		size_t split = str.find_last_of(' ', 49);
		while(string_length(str.substr(0,split), buf_style) >= width)
			split = str.find_last_of(' ', split - 1);
		add_string_to_buf(str.substr(0,split));
		str = str.substr(split);
		std::string space(indent, ' ');
		size_t wrap_w = width - string_length(space, buf_style);
		while(string_length(str.substr(0,str.find_last_not_of(' ')), buf_style) > wrap_w) {
			std::string wrap = space;
			split = str.find_last_of(' ', 49 - indent);
			while(string_length(str.substr(0,split), buf_style) >= wrap_w)
				split = str.find_last_of(' ', split - 1);
			wrap += str.substr(0,split);
			str = str.substr(split);
			add_string_to_buf(wrap);
		}
		add_string_to_buf(space + str);
		return;
	}
	
	// Now check if this is a duplicate message
	int prev_pointer = buf_pointer - 1;
	if(prev_pointer < 0) prev_pointer = TEXT_BUF_LEN - 1;
	size_t last = 0, new_last = str.find_last_not_of(' ');
	while(last < str.length() && str[last] == text_buffer[prev_pointer].line[last])
		last++;
	while(text_buffer[prev_pointer].line[--last] == ' ');
	bool is_dup;
	if(last == new_last) {
		size_t i, num_pos = 0;
		enum {begin, f_space, f_lparen, f_x, f_num, f_rparen, err} state = begin;
		for(i = last; i < 50 && text_buffer[prev_pointer].line[i]; i++) {
			if(state == f_x) num_pos = i;
			if(isdigit(text_buffer[prev_pointer].line[i]) && (state == f_x || state == f_num))
				state = f_num;
			else switch(text_buffer[prev_pointer].line[i]) {
				case ' ':
					if(state == begin || state == f_space)
						state = f_space;
					break;
				case '(':
					if(state == begin || state == f_space)
						state = f_lparen;
					break;
				case 'x':
					if(state == f_lparen)
						state = f_x;
					break;
				case ')':
					if(state == f_num)
						state = f_rparen;
					break;
				default:
					if(i > last)
						state = err;
					break;
			}
			if(state == f_rparen) break;
		}
		if(state == begin || state == f_space || state == f_rparen) {
			is_dup = true;
			last++;
		}
		if(is_dup) {
			int lastCount = 1;
			if(num_pos > 0)
				sscanf(text_buffer[prev_pointer].line + num_pos, "%d", &lastCount);
			
			sprintf(text_buffer[prev_pointer].line + last, " (x%d)", lastCount + 1);
			return;
		}
	}
	
	text_sbar->setPosition(58); // TODO: This seems oddly specific
	if(buf_pointer == mark_where_printing_long) {
		printing_long = true;
		print_buf();
		through_sending();
	}
	sprintf((char *)text_buffer[buf_pointer].line, "%-49.49s", str.c_str());
//	c2pstr((char *)text_buffer[buf_pointer].line);
	if(buf_pointer == (TEXT_BUF_LEN - 1))
		buf_pointer = 0;
	else buf_pointer++;
	
}

void init_buf() {
	short i;
	
	for(i = 0; i < TEXT_BUF_LEN; i++)
		sprintf((char *) text_buffer[buf_pointer].line, " ");
}

void print_buf () {
	short num_lines_printed = 0;
	long ctrl_val;
	short line_to_print;
	long start_print_point;
	bool end_loop = false;
	rectangle store_text_rect,dest_rect,erase_rect = {2,2,136,255};
	
	text_area_gworld.setActive();
	
	// First clean up gworld with pretty patterns
	tileImage(text_area_gworld, erase_rect,bg[6]);
	
	ctrl_val = 58 - text_sbar->getPosition();
	start_print_point = buf_pointer - LINES_IN_TEXT_WIN - ctrl_val;
	if(start_print_point< 0)
		start_print_point= TEXT_BUF_LEN + start_print_point;
	line_to_print= start_print_point;
	
	location moveTo;
	while((line_to_print!= buf_pointer) && (num_lines_printed < LINES_IN_TEXT_WIN)) {
		moveTo = location(4, 1 + 12 * num_lines_printed);
		sf::Text text(text_buffer[line_to_print].line, *ResMgr::get<FontRsrc>("plain"), 12);
		text.setColor(sf::Color::Black);
		text.setPosition(moveTo);
		text_area_gworld.draw(text);
		num_lines_printed++;
		line_to_print++;
		if(line_to_print== TEXT_BUF_LEN) {
			line_to_print= 0;
		}
		
		if((num_lines_printed == LINES_IN_TEXT_WIN - 1) && (printing_long)) {
			end_loop = false;
			line_to_print= buf_pointer;
		}
		
	}
	
	text_area_gworld.display();
}

void restart_printing() {
	lines_to_print = 0;
	//clear_text_panel();
}

void restore_mode() {
	overall_mode = store_mode;
}

void through_sending() {
	mark_where_printing_long = buf_pointer + LINES_IN_TEXT_WIN - 1;
	if(mark_where_printing_long > TEXT_BUF_LEN - 1)
		mark_where_printing_long -= TEXT_BUF_LEN;
	printing_long = false;
}

/* Draw a bitmap in the world window. hor in 0 .. 8, vert in 0 .. 8,
 object is ptr. to bitmap to be drawn, and masking is for Copybits. */
void Draw_Some_Item (sf::Texture& src_gworld, rectangle src_rect, sf::RenderTarget& targ_gworld,location target, char masked, short main_win) {
	rectangle	destrec = {0,0,36,28};
	
	if((target.x < 0) || (target.y < 0) || (target.x > 8) || (target.y > 8))
		return;
	if((supressing_some_spaces) && (target != ok_space[0]) &&
		(target != ok_space[1]) && (target != ok_space[2]) && (target != ok_space[3]))
		return;
	terrain_there[target.x][target.y] = -1;
	
	destrec = coord_to_rect(target.x,target.y);
	if(main_win == 1) destrec.offset(ul.x + 5,ul.y + 5);
	
	if(main_win == 0) {
		if(masked == 1)
			rect_draw_some_item(src_gworld, src_rect, targ_gworld, destrec, sf::BlendAlpha);
		else rect_draw_some_item(src_gworld, src_rect, targ_gworld, destrec, sf::BlendNone);
	} else {
		if(masked == 1)
			rect_draw_some_item(src_gworld, src_rect, targ_gworld, destrec, sf::BlendAlpha);
		else rect_draw_some_item(src_gworld, src_rect, targ_gworld, destrec, sf::BlendNone);
	}
}

// TODO: This seems to duplicate logic found in graphtool to get a rect from a picture index
rectangle coord_to_rect(short i,short j) {
	rectangle to_return;
	
	to_return.left = 13 + BITMAP_WIDTH * i;
	to_return.right = to_return.left + BITMAP_WIDTH;
	to_return.top = 13 + BITMAP_HEIGHT * j;
	to_return.bottom = to_return.top + BITMAP_HEIGHT;
	
	return to_return;
}


void make_cursor_sword() {
	set_cursor(sword_curs);
}

short calc_day() {
	return (short) ((univ.party.age) / 3700) + 1;
}

// which_day is day event should happen
// which_event is the univ.party.key_times value to cross reference with.
// if the key_time is reached before which_day, event won't happen
// if it's 0, event always happens
bool day_reached(unsigned short which_day, unsigned short which_event) {
	// Windows version unconditionally added 20 days for no reason at all.
	// Instead, let's add 10 days, but only if easy mode enabled.
	if(PSD[SDF_EASY_MODE]) which_day += 10;
	if(which_event > 10)
		return false;
	if((which_event > 0) && (univ.party.key_times[which_event] < which_day))
		return false;
	if(calc_day() >= which_day)
		return true;
	else return false;
}
