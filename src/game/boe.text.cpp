
const int LINES_IN_TEXT_WIN = 11;
const int TEXT_BUF_LEN = 70;

#include <sstream>
#include <list>

#include "boe.global.hpp"
#include "boe.graphutil.hpp"
#include "universe.hpp"
#include "boe.text.hpp"
#include "boe.locutils.hpp"
#include "mathutil.hpp"
#include "render_text.hpp"
#include "render_image.hpp"
#include "render_shapes.hpp"
#include "tiling.hpp"
#include "utility.hpp"
#include "scrollbar.hpp"
#include "res_image.hpp"
#include "res_font.hpp"
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

rectangle item_buttons_from[7] = {
	{11,0,24,14},{11,14,24,28},{11,28,24,42},{11,42,24,56},
	{24,0,36,30},{24,30,36,60},{36,0,48,30}
};

eGameMode store_mode;

extern short had_text_freeze;
extern eStatMode stat_screen_mode;

// graphics globals
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
extern location center;

extern cCustomGraphics spec_scen_g;
extern sf::RenderTexture pc_stats_gworld, item_stats_gworld, text_area_gworld;
extern sf::RenderTexture terrain_screen_gworld;

// game globals
extern location ul;
extern rectangle item_buttons[8][6];
// name, use, give, drip, info, sell/id
extern rectangle pc_buttons[6][5];
// name, hp, sp, info, trade
extern bool item_area_button_active[8][6];
extern bool pc_area_button_active[6][5];
extern rectangle item_screen_button_rects[9];
extern std::vector<int> spec_item_array;
extern std::map<eItemAbil, short> abil_chart;
// combat globals
extern short item_bottom_button_active[9];
extern cUniverse univ;
extern short shop_identify_cost;
extern short store_selling_values[8];
extern short combat_posing_monster, current_working_monster; // 0-5 PC 100 + x - monster x
extern bool supressing_some_spaces;
extern location ok_space[4];
extern std::map<eStatus, std::pair<short, short>> statIcons;

// Draws the pc area in upper right
void put_pc_screen() {
	rectangle erase_rect = {17,2,98,269},to_draw_rect,from_rect;
	rectangle food_rect[2] = {{103,34,114,76}, {103,3,114,40}};
	rectangle gold_rect[2] = {{103,106,114,147}, {103,75,114,104}};
	rectangle day_rect[2] = {{103,174,114,201}, {103,147,114,172}};
	rectangle title_rects[3] = {{4,4,16,180}, {4,184,16,214}, {4,214,16,237}};
	rectangle bottom_bar_rect = {99,0,116,271};
	rectangle info_from = {0,1,12,13};
	
	pc_stats_gworld.setActive();
	
	// First clean up gworld with pretty patterns
	sf::Texture& orig = *ResMgr::get<ImageRsrc>("statarea");
	rect_draw_some_item(orig, rectangle(orig), pc_stats_gworld, rectangle(pc_stats_gworld));
	tileImage(pc_stats_gworld, erase_rect,bg[6]);
	
	TextStyle style;
	style.font = FONT_BOLD;
	style.pointSize = 10;
	style.colour = sf::Color::Yellow;
	style.lineHeight = 10;
	win_draw_string(pc_stats_gworld,food_rect[1],"Food:",eTextMode::WRAP,style);
	win_draw_string(pc_stats_gworld,gold_rect[1],"Gold:",eTextMode::WRAP,style);
	win_draw_string(pc_stats_gworld,day_rect[1],"Day:",eTextMode::WRAP,style);
	win_draw_string(pc_stats_gworld,title_rects[0],"Party stats:",eTextMode::WRAP,style);
	win_draw_string(pc_stats_gworld,title_rects[1],"HP:",eTextMode::WRAP,style);
	win_draw_string(pc_stats_gworld,title_rects[2],"SP:",eTextMode::WRAP,style);
	
	style.colour = sf::Color::White;
	style.pointSize = 12;
	// Put food, gold, day
	win_draw_string(pc_stats_gworld,food_rect[0],std::to_string(univ.party.food),eTextMode::WRAP,style);
	win_draw_string(pc_stats_gworld,gold_rect[0],std::to_string(univ.party.gold),eTextMode::WRAP,style);
	win_draw_string(pc_stats_gworld,day_rect[0],std::to_string(univ.party.calc_day()),eTextMode::WRAP,style);
	style.colour = sf::Color::Black;
	
	sf::Texture& invenbtn_gworld = *ResMgr::get<ImageRsrc>("invenbtns");
	for(short i = 0; i < 6; i++) {
		if(univ.party[i].main_status != eMainStatus::ABSENT) {
			for(short j = 0; j < 5; j++)
				pc_area_button_active[i][j] = 1;
			if(i == univ.cur_pc) {
				style.italic = true;
				style.colour = sf::Color::Blue;
			}
			
			std::ostringstream sout;
			sout << i + 1 << ". " << univ.party[i].name;
			win_draw_string(pc_stats_gworld,pc_buttons[i][PCBTN_NAME],sout.str(),eTextMode::WRAP,style);
			style.italic = false;
			style.colour = sf::Color::Black;
			
			to_draw_rect = pc_buttons[i][PCBTN_HP];
			to_draw_rect.right += 20;
			sout.str("");
			switch(univ.party[i].main_status) {
				case eMainStatus::ALIVE:
					if(univ.party[i].cur_health == univ.party[i].max_health)
						style.colour = sf::Color::Green;
					else if(univ.party[i].cur_health > univ.party[i].max_health)
						style.colour = {0xff,0x80,0}; // Orange
					else style.colour = sf::Color::Red;
					win_draw_string( pc_stats_gworld,pc_buttons[i][PCBTN_HP],std::to_string(univ.party[i].cur_health),eTextMode::WRAP,style);
					if(univ.party[i].cur_sp == univ.party[i].max_sp)
						style.colour = sf::Color::Blue;
					else if(univ.party[i].cur_sp > univ.party[i].max_sp)
						style.colour = {0,0xff,0x80}; // Teal
					else style.colour = sf::Color::Magenta;
					win_draw_string( pc_stats_gworld,pc_buttons[i][PCBTN_SP],std::to_string(univ.party[i].cur_sp),eTextMode::WRAP,style);
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
			to_draw_rect = pc_buttons[i][PCBTN_INFO];
			to_draw_rect.right = pc_buttons[i][PCBTN_TRADE].right + 1;
			from_rect = info_from;
			from_rect.right = from_rect.left + to_draw_rect.right - to_draw_rect.left;
			rect_draw_some_item(invenbtn_gworld,from_rect,pc_stats_gworld,to_draw_rect,sf::BlendAlpha);
		}
		else {
			for(short j = 0; j < 5; j++)
				pc_area_button_active[i][j] = 0;
		}
	}
	
	rectangle help_from_rect = {46,60,59,76};
	to_draw_rect = {101,251,114,267};
	rect_draw_some_item(invenbtn_gworld, help_from_rect, pc_stats_gworld, to_draw_rect, sf::BlendAlpha);
	
	pc_stats_gworld.display();
	
	// Sometimes this gets called when character is slain. when that happens, if items for
	// that PC are up, switch item page.
	if(univ.cur_pc < 6 && univ.current_pc().main_status != eMainStatus::ALIVE && stat_window == univ.cur_pc) {
		set_stat_window(univ.cur_pc);
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
void put_item_screen(short screen_num) {
	std::ostringstream sout;
	long i_num;
	long item_offset;
	short pc;
	rectangle erase_rect = {17,2,122,255},dest_rect;
	rectangle upper_frame_rect = {3,3,15,268};
	
	item_stats_gworld.setActive();
	
	// First clean up gworld with pretty patterns
	sf::Texture& orig = *ResMgr::get<ImageRsrc>("inventory");
	rect_draw_some_item(orig, rectangle(orig), item_stats_gworld, rectangle(item_stats_gworld));
	tileImage(item_stats_gworld, erase_rect,bg[6]);
	
	// Draw buttons at bottom
	item_offset = item_sbar->getPosition();
	
	for(short i = 0; i < 8; i++)
		for(short j = 0; j < 6; j++)
			item_area_button_active[i][j] = false;
	
	TextStyle style;
	style.lineHeight = 10;
	style.font = FONT_BOLD;
	style.colour = sf::Color::Yellow;
	switch(screen_num) {
		case ITEM_WIN_SPECIAL:
			win_draw_string(item_stats_gworld,upper_frame_rect,"Special items:",eTextMode::WRAP,style);
			style.colour = sf::Color::Black;
			for(short i = 0; i < 8; i++) {
				i_num = i + item_offset;
				if(i_num < spec_item_array.size()) {
					win_draw_string(item_stats_gworld,item_buttons[i][ITEMBTN_NAME],univ.scenario.special_items[spec_item_array[i_num]].name,eTextMode::WRAP,style);
					
					place_item_button(3,i,4,0);
					if((univ.scenario.special_items[spec_item_array[i_num]].flags % 10 == 1)
						&& (!(is_combat())))
						place_item_button(0,i,3,0);
				}
			}
			break;
		case ITEM_WIN_QUESTS:
			win_draw_string(item_stats_gworld,upper_frame_rect,"Quests/Jobs:",eTextMode::WRAP,style);
			style.colour = sf::Color::Black;
			for(short i = 0; i < 8; i++) {
				i_num = i + item_offset;
				if(i_num < spec_item_array.size()) {
					int which_quest = spec_item_array[i_num] % 10000;
					if(spec_item_array[i_num] / 10000 == 2)
						style.colour = sf::Color::Red;
					
					win_draw_string(item_stats_gworld,item_buttons[i][ITEMBTN_NAME],univ.scenario.quests[which_quest].name,eTextMode::WRAP,style);
					
					if(spec_item_array[i_num] / 10000 == 1) {
						location from, to;
						from = to = item_buttons[i][ITEMBTN_NAME].centre();
						from.x = item_buttons[i][ITEMBTN_NAME].left;
						to.x = from.x + string_length(univ.scenario.quests[which_quest].name, style);
						draw_line(item_stats_gworld, from, to, 1, sf::Color::Green);
					}
					
					place_item_button(3,i,4,0);
				}
			}
			break;
			
		default: // on an items page
			pc = screen_num;
			sout.str("");;
			sout << univ.party[pc].name << " inventory:",
			win_draw_string(item_stats_gworld,upper_frame_rect,sout.str(),eTextMode::WRAP,style);
			style.colour = sf::Color::Black;
			
			for(short i = 0; i < 8; i++) {
				i_num = i + item_offset;
				sout.str("");
				sout << i_num + 1 << '.';
				win_draw_string(item_stats_gworld,item_buttons[i][ITEMBTN_NAME],sout.str(),eTextMode::WRAP,style);
				
 				dest_rect = item_buttons[i][ITEMBTN_NAME];
				dest_rect.left += 36;
				dest_rect.top -= 2;
				
				const cPlayer& who = univ.party[pc];
				const cItem& item = who.items[i_num];
				
				if(item.variety != eItemType::NO_ITEM) {
					style.font = FONT_PLAIN;
					if(who.equip[i_num]) {
						style.italic = true;
						if(item.variety == eItemType::ONE_HANDED || item.variety == eItemType::TWO_HANDED)
							style.colour = sf::Color::Magenta;
						else if((*item.variety).is_armour)
							style.colour = sf::Color::Green;
						else style.colour = sf::Color::Blue;
					} else style.colour = sf::Color::Black;
					
					sout.str("");
					if(!item.ident)
						sout << item.name << "  ";
					else { /// Don't place # of charges when Sell button up and space tight
						sout << item.full_name << ' ';
						if(item.charges > 0 && item.ability != eItemAbil::MESSAGE && (stat_screen_mode == MODE_INVEN || stat_screen_mode == MODE_SHOP))
							sout << '(' << int(item.charges) << ')';
					}
					dest_rect.left -= 2;
					win_draw_string(item_stats_gworld,dest_rect,sout.str(),eTextMode::WRAP,style);
					style.italic = false;
					style.colour = sf::Color::Black;
					
					// this is kludgy, awkwark, and has redundant code. Done this way to
					// make go faster, and I got lazy.
					if((stat_screen_mode == MODE_SHOP) &&
						((is_town()) || (is_out()) || ((is_combat()) && (pc == univ.cur_pc)))) { // place give and drop and use
						place_item_button(0,i,0,univ.party[pc].items[i_num].graphic_num); // item_graphic
						if(abil_chart[univ.party[pc].items[i_num].ability]) // place use if can
							place_item_button(ITEMBTN_NORM,i,1,0);
						else place_item_button(ITEMBTN_ALL,i,1,0);
					}
					else {
						place_item_button(0,i,0,univ.party[pc].items[i_num].graphic_num); // item_graphic
						place_item_button(3,i,4,0); // info button
						if((stat_screen_mode == MODE_INVEN) &&
							((is_town()) || (is_out()) || ((is_combat()) && (pc == univ.cur_pc)))) { // place give and drop and use
							place_item_button(1,i,2,0);
							place_item_button(2,i,3,0);
							if(abil_chart[item.ability]) // place use if can
								place_item_button(0,i,1,0);
						}
					}
					if(stat_screen_mode != MODE_INVEN && stat_screen_mode != MODE_SHOP) {
						place_buy_button(i,pc,i_num);
						
					}
					
				} // end of if item is there
			} // end of for(short i = 0; i < 8; i++)
			break;
	}
	
	place_item_bottom_buttons();
	item_stats_gworld.display();
}

void place_buy_button(short position,short pc_num,short item_num) {
	rectangle dest_rect,source_rect;
	rectangle button_sources[3] = {{24,0,36,30},{36,0,48,30},{48,0,60,30}};
	short val_to_place;
	// TODO: This is now duplicated here and in start_town_mode()
	short aug_cost[10] = {4,7,10,8, 15,15,10, 0,0,0};
	
	const cPlayer& pc = univ.party[pc_num];
	const cItem& item = pc.items[item_num];
	
	if(item.variety == eItemType::NO_ITEM)
		return;
	
	dest_rect = item_buttons[position][5];
	
	val_to_place = (item.charges > 0) ?
		item.charges * item.value :
		item.value;
	val_to_place = val_to_place / 2;
	
	switch(stat_screen_mode) {
		case MODE_IDENTIFY:
			if(!item.ident) {
				item_area_button_active[position][5] = true;
				source_rect = button_sources[0];
				val_to_place = shop_identify_cost;
			}
			break;
		case MODE_SELL_WEAP:
			if((*item.variety).is_weapon && !pc.equip[item_num] && item.ident && val_to_place > 0 && !item.unsellable) {
				item_area_button_active[position][5] = true;
				source_rect = button_sources[1];
			}
			break;
		case MODE_SELL_ARMOR:
			if((*item.variety).is_armour && !pc.equip[item_num] && item.ident && val_to_place > 0 && !item.unsellable) {
				item_area_button_active[position][5] = true;
				source_rect = button_sources[1];
			}
			break;
		case MODE_SELL_ANY:
			if(!pc.equip[item_num] && item.ident && val_to_place > 0 && !item.unsellable) {
				item_area_button_active[position][5] = true;
				source_rect = button_sources[1];
			}
			break;
		case MODE_ENCHANT:
			if((item.variety == eItemType::ONE_HANDED || item.variety == eItemType::TWO_HANDED) && item.ident && item.ability == eItemAbil::NONE && !item.magic) {
				item_area_button_active[position][5] = true;
				source_rect = button_sources[2];
				val_to_place = max(aug_cost[shop_identify_cost] * 100, item.value * (5 + aug_cost[shop_identify_cost]));
			}
			break;
		case MODE_INVEN: case MODE_SHOP:
			// These modes don't have buy buttons, so we shouldn't get here; bail out!
			return;
	}
	if(item_area_button_active[position][5]) {
		sf::Texture& invenbtn_gworld = *ResMgr::get<ImageRsrc>("invenbtns");
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
		else rect_draw_some_item(*ResMgr::get<ImageRsrc>("tinyobj"), from_rect, item_stats_gworld, to_rect, sf::BlendAlpha);
		return;
	}
	sf::Texture& invenbtn_gworld = *ResMgr::get<ImageRsrc>("invenbtns");
	if(which_button_to_put < 4) { // this means put a regular item button
		item_area_button_active[which_slot][which_button_position] = true;
		rect_draw_some_item(invenbtn_gworld, item_buttons_from[which_button_to_put], item_stats_gworld, item_buttons[which_slot][which_button_position], sf::BlendAlpha);
	}
	if(which_button_to_put == ITEMBTN_ALL) { // this means put all 4
		item_area_button_active[which_slot][1] = true;
		item_area_button_active[which_slot][2] = true;
		item_area_button_active[which_slot][3] = true;
		item_area_button_active[which_slot][4] = true;
		from_rect = item_buttons_from[0];
		from_rect.right = item_buttons_from[3].right;
		to_rect = item_buttons[which_slot][1];
		to_rect.right = to_rect.left + from_rect.right - from_rect.left;
		rect_draw_some_item(invenbtn_gworld, from_rect, item_stats_gworld, to_rect, sf::BlendAlpha);
	}
	if(which_button_to_put == ITEMBTN_NORM) { // this means put right 3
		item_area_button_active[which_slot][2] = true;
		item_area_button_active[which_slot][3] = true;
		item_area_button_active[which_slot][4] = true;
		from_rect = item_buttons_from[1];
		from_rect.right = item_buttons_from[3].right;
		to_rect = item_buttons[which_slot][2];
		to_rect.right = to_rect.left + from_rect.right - from_rect.left;
		rect_draw_some_item(invenbtn_gworld, from_rect, item_stats_gworld, to_rect, sf::BlendAlpha);
	}
}

void place_item_bottom_buttons() {
	rectangle pc_from_rect = {0,0,36,28},but_from_rect = {30,60,46,78},to_rect;
	rectangle spec_from_rect = {0,60,15,95}, job_from_rect = {15,60,30,95}, help_from_rect = {46,60,59,76};
	// TODO: What about when the buttons are pressed?
	TextStyle style;
	style.lineHeight = 10;
	style.pointSize = 12;
	style.font = FONT_BOLD;
	style.colour = sf::Color::Yellow;
	
	sf::Texture& invenbtn_gworld = *ResMgr::get<ImageRsrc>("invenbtns");
	for(short i = 0; i < 6; i++) {
		if(univ.party[i].main_status == eMainStatus::ALIVE) {
		 	item_bottom_button_active[i] = true;
		 	to_rect = item_screen_button_rects[i];
			rect_draw_some_item(invenbtn_gworld, but_from_rect, item_stats_gworld, to_rect, sf::BlendAlpha);
			pic_num_t pic = univ.party[i].which_graphic;
			sf::Texture* from_gw;
			if(pic >= 1000) {
				bool isParty = pic >= 10000;
				pic_num_t need_pic = pic % 1000;
				graf_pos_ref(from_gw, pc_from_rect) = spec_scen_g.find_graphic(need_pic, isParty);
			} else if(pic >= 100) {
				// Note that we assume it's a 1x1 graphic.
				// PCs can't be larger than that, but we leave it to the scenario designer to avoid assigning larger graphics.
				pic_num_t need_pic = pic - 100;
				int mode = 0;
				pc_from_rect = get_monster_template_rect(need_pic, mode, 0);
				int which_sheet = m_pic_index[need_pic].i / 20;
				from_gw = ResMgr::get<ImageRsrc>("monst" + std::to_string(1 + which_sheet)).get();
			} else {
				pc_from_rect = calc_rect(2 * (pic / 8), pic % 8);
				from_gw = ResMgr::get<ImageRsrc>("pcs").get();
			}
			to_rect.inset(2,2);
			rect_draw_some_item(*from_gw, pc_from_rect, item_stats_gworld, to_rect, sf::BlendAlpha);
			std::string numeral = std::to_string(i + 1);
			short width = string_length(numeral, style);
			// Offset "6" down two pixels to make it line up, because it has an ascender in this font
			// Offset "1" - "4" down as well because they're not shorter and it looks a bit better
			to_rect.offset(-width - 5, i != 4 ? 2 : 0);
			win_draw_string(item_stats_gworld, to_rect, numeral, eTextMode::LEFT_TOP, style);
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
	short array_pos = 0;
	
	stat_window = new_stat;
	if(stat_window < ITEM_WIN_SPECIAL && univ.party[stat_window].main_status != eMainStatus::ALIVE)
		stat_window = first_active_pc();
	item_sbar->setPageSize(8);
	spec_item_array.clear();
	switch(stat_window) {
		case ITEM_WIN_SPECIAL:
			std::fill(spec_item_array.begin(), spec_item_array.end(), -1);
			for(short i = 0; i < univ.scenario.special_items.size(); i++)
				if(univ.party.spec_items.count(i)) {
					spec_item_array.push_back(i);
					array_pos++;
				}
			array_pos = max(0,array_pos - 8);
			item_sbar->setMaximum(array_pos);
			break;
		case ITEM_WIN_QUESTS:
			std::fill(spec_item_array.begin(), spec_item_array.end(), -1);
			for(short i = 0; i < univ.scenario.quests.size(); i++)
				if(univ.party.active_quests[i].status == eQuestStatus::STARTED) {
					spec_item_array.push_back(i);
					array_pos++;
				} else if(univ.party.active_quests[i].status == eQuestStatus::COMPLETED) {
					spec_item_array.push_back(i + 10000);
					array_pos++;
				} else if(univ.party.active_quests[i].status == eQuestStatus::FAILED) {
					spec_item_array.push_back(i + 20000);
					array_pos++;
				}
			array_pos = max(0,array_pos - 8);
			item_sbar->setMaximum(array_pos);
			break;
		default:
			item_sbar->setMaximum(16);
			break;
	}
	item_sbar->setPosition(0);
	put_item_screen(stat_window);
	
}

short first_active_pc() {
	for(short i = 0; i < 6; i++)
		if(univ.party[i].main_status == eMainStatus::ALIVE)
			return i;
	return 6;
}


void refresh_stat_areas(short mode) {
	sf::BlendMode x;
	rectangle dest_rect,parts_of_area_to_draw[3] = {{0,0,17,271},{16,0,123,256},{123,0,144,271}};
	
	dest_rect = rectangle(pc_stats_gworld);
	rectangle oldRect = dest_rect;
	dest_rect.offset(PC_WIN_UL_X,PC_WIN_UL_Y);
	if(mode == 1) x = sf::BlendAdd;
	else x = sf::BlendNone;
	rect_draw_some_item (pc_stats_gworld.getTexture(), oldRect, dest_rect,ul, x);
	for(short i = 0; i < 3; i++) {
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

short total_encumbrance(short pc_num) {
	short store = 0,what_val;
	
	for(short i = 0; i < univ.party[pc_num].items.size(); i++)
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
	rectangle dest_rect = {18,15,30,27};
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
	sf::Texture& status_gworld = *ResMgr::get<ImageRsrc>("staticons");
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
		add_string_to_buf("  Kills in this town: " + std::to_string(univ.town->m_killed));
	}
	add_string_to_buf("  Total experience: " + std::to_string(univ.party.total_xp_gained));
	add_string_to_buf("  Total damage done: " + std::to_string(univ.party.total_dam_done));
	add_string_to_buf("  Total damage taken: " + std::to_string(univ.party.total_dam_taken));
	print_buf();
}


short do_look(location space) {
	short num_items = 0;
	bool gold_here = false, food_here = false, is_lit = true;
	location from_where;
	std::string msg;
	
	from_where = get_cur_loc();
	is_lit = is_out() || pt_in_light(from_where,space);
	
	if((overall_mode == MODE_LOOK_OUTDOORS && space == univ.party.out_loc) ||
		(overall_mode == MODE_LOOK_TOWN && space == univ.party.town_loc))
		add_string_to_buf("    Your party");
	if(overall_mode == MODE_LOOK_COMBAT)
		for(short i = 0; i < 6; i++)
			if(space == univ.party[i].combat_pos && univ.party[i].main_status == eMainStatus::ALIVE
			   && (is_lit) && (can_see_light(univ.current_pc().combat_pos,space,sight_obscurity) < 5)) {
				msg = "    " + univ.party[i].name;
				add_string_to_buf(msg);
			}
	
	if((overall_mode == MODE_LOOK_TOWN) || (overall_mode == MODE_LOOK_COMBAT)) {
		for(short i = 0; i < univ.town.monst.size(); i++)
			if((univ.town.monst[i].active != 0) && (is_lit)
				&& univ.town.monst[i].on_space(space) &&
				((overall_mode == MODE_LOOK_TOWN) || (can_see_light(univ.current_pc().combat_pos,space,sight_obscurity) < 5))
				&& (univ.town.monst[i].picture_num != 0)) {
				
				
				msg = "    ";
				if(univ.town.monst[i].health < univ.town.monst[i].m_health)
					msg += "Wounded ";
				msg += get_m_name(univ.town.monst[i].number);
				msg += univ.town.monst[i].is_friendly() ? " (F)" : " (H)";
				
				add_string_to_buf(msg.c_str());
				
			}
	}
	if(overall_mode == MODE_LOOK_OUTDOORS) {
		for(short i = 0; i < 10; i++) {
			if((univ.party.out_c[i].exists)
				&& (space == univ.party.out_c[i].m_loc)) {
				for(short j = 0; j < 7; j++)
					if(univ.party.out_c[i].what_monst.monst[j] != 0) {
						msg = get_m_name(univ.party.out_c[i].what_monst.monst[j]);
						msg = "    " + msg;
						add_string_to_buf(msg.c_str());
						j = 7;
						
					}
				
			}
		}
		
		if(univ.out->roads[space.x][space.y])
			add_string_to_buf("    Road");
		if(out_boat_there(space))
			add_string_to_buf("    Boat");
		if(out_horse_there(space))
			add_string_to_buf("    Horse");
		if(univ.out->special_spot[space.x][space.y])
			add_string_to_buf("    Special Encounter");
	}
	
	if((overall_mode == MODE_LOOK_TOWN) || (overall_mode == MODE_LOOK_COMBAT)) {
		if(univ.town.is_road(space.x,space.y))
			add_string_to_buf("    Track");
		if(town_boat_there(space))
			add_string_to_buf("    Boat");
		if(town_horse_there(space))
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
		if(univ.town.is_sleep_cloud(space.x,space.y))
			add_string_to_buf("    Sleep Cloud");
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
		
		for(short i = 0; i < univ.town.items.size(); i++) {
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
		else for(short i = 0; i < univ.town.items.size(); i++) {
			if(univ.town.items[i].variety != eItemType::NO_ITEM && univ.town.items[i].variety != eItemType::GOLD && univ.town.items[i].variety != eItemType::FOOD &&
			   (space == univ.town.items[i].item_loc) && (!univ.town.items[i].contained)) {
				if(univ.town.items[i].ident)
					msg = "    " + univ.town.items[i].full_name;
				else msg = "    " + univ.town.items[i].name;
				add_string_to_buf((char *) msg.c_str());
			}
		}
		if(univ.town.is_spot(space.x,space.y))
			add_string_to_buf("    Special Encounter");
	}
	
	if(!is_lit) {
		add_string_to_buf("    Dark");
		return 0;
	}
	
	return print_terrain(space);
}

cVehicle* town_boat_there(location where) {
	for(short i = 0; i < univ.party.boats.size(); i++)
		if(univ.party.boats[i].exists && univ.party.boats[i].which_town == univ.party.town_num
			&& (where == univ.party.boats[i].loc))
			return &univ.party.boats[i];
	return nullptr;
}
cVehicle* out_boat_there(location where) {
	where = global_to_local(where);
	for(short i = 0; i < univ.party.boats.size(); i++)
		if((univ.party.boats[i].exists) && (where == univ.party.boats[i].loc)
			&& (univ.party.boats[i].which_town == 200))
			return &univ.party.boats[i];
	return nullptr;
}

cVehicle* town_horse_there(location where) {
	for(short i = 0; i < univ.party.horses.size(); i++)
		if(univ.party.horses[i].exists && univ.party.horses[i].which_town == univ.party.town_num
			&& (where == univ.party.horses[i].loc))
			return &univ.party.horses[i];
	return nullptr;
}
cVehicle* out_horse_there(location where) {
	where = global_to_local(where);
	for(short i = 0; i < univ.party.horses.size(); i++)
		if((univ.party.horses[i].exists) && (where == univ.party.horses[i].loc)
			&& (univ.party.horses[i].which_town == 200))
			return &univ.party.horses[i];
	return nullptr;
}
void notify_out_combat_began(cOutdoors::cWandering encounter,short *nums) {
	std::string msg;
	
	add_string_to_buf("COMBAT!");
	
	for(short i = 0; i < 6; i++)
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
	bool is_dup = false;
	if(last == new_last) {
		size_t num_pos = 0;
		enum {begin, f_space, f_lparen, f_x, f_num, f_rparen, err} state = begin;
		for(short i = last; i < 50 && text_buffer[prev_pointer].line[i]; i++) {
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
	if(buf_pointer == (TEXT_BUF_LEN - 1))
		buf_pointer = 0;
	else buf_pointer++;
	
}

void init_buf() {
	for(short i = 0; i < TEXT_BUF_LEN; i++)
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

std::list<text_label_t> posted_labels;

void place_text_label(std::string string, location at, bool centred) {
	TextStyle style;
	style.font = FONT_PLAIN;
	short height = 0;
	short width = string_length(string, style, &height);
	at.x -= center.x - 4;
	at.x *= 28;
	at.x += 14;
	at.x -= width / 2;
	
	at.y -= center.y - 4;
	at.y *= 36;
	if(centred)
		at.y += 18;
	if(at.y == 0) at.y = 36;
	else at.y -= height;
	
	rectangle text_rect(at, loc(at.x + width, at.y + height));
	text_rect.offset(-min(at.x,0),-min(at.y,0)); // If it's longer, make it off-centre to keep it onscreen.
	text_rect.offset(13,13);
	posted_labels.push_back({text_rect, string});
}

void draw_text_label(const text_label_t& label) {
	sf::Color back_clr = {64, 64, 64, 42};
	TextStyle style;
	style.font = FONT_PLAIN;
	style.colour = sf::Color::White;
	rectangle back_rect = label.text_rect, text_rect = label.text_rect;
	back_rect.inset(-7,-7);
	back_rect.offset(0,-2);
	for(int i = 0; i <= 3; i++) {
		fill_roundrect(terrain_screen_gworld, back_rect, 7, back_clr);
		back_rect.inset(2,2);
		back_clr.a *= 1.5;
	}
	//text_rect.offset(0, -text_rect.height() + 1);
	text_rect.offset(0, -5);
	win_draw_string(terrain_screen_gworld, text_rect, label.str, eTextMode::LEFT_TOP, style);
}

// TODO: Replace uses of this function with direct use of calc_rect
rectangle coord_to_rect(short i,short j) {
	rectangle to_return = calc_rect(i, j);
	to_return.offset(13, 13);
	return to_return;
}

// which_day is day event should happen
// which_event is the univ.party.key_times value to cross reference with.
// if the key_time is reached before which_day, event won't happen
// if it's 0, event always happens
bool day_reached(unsigned short which_day, unsigned short which_event) {
	// Windows version unconditionally added 20 days for no reason at all.
	// Instead, let's add 10 days, but only if easy mode enabled.
	if(univ.party.easy_mode) which_day += 10;
	if(which_event > 0) {
		if(univ.party.key_times.find(which_event) == univ.party.key_times.end())
			return false;
		if(univ.party.key_times[which_event] < which_day)
			return false;
	}
	if(univ.party.calc_day() >= which_day)
		return true;
	else return false;
}
