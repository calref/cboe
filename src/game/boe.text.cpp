
const int LINES_IN_TEXT_WIN = 11;
const int TEXT_BUF_LEN = 70;

const int LINES_IN_ITEM_WIN = 8;

#include <sstream>
#include <list>

#include "boe.global.hpp"
#include "boe.graphics.hpp"
#include "boe.graphutil.hpp"
#include "universe/universe.hpp"
#include "boe.text.hpp"
#include "boe.locutils.hpp"
#include "boe.infodlg.hpp"
#include "mathutil.hpp"
#include "gfx/render_text.hpp"
#include "gfx/render_image.hpp"
#include "gfx/render_shapes.hpp"
#include "gfx/tiling.hpp"
#include "utility.hpp"
#include "dialogxml/widgets/scrollbar.hpp"
#include "dialogxml/dialogs/strdlog.hpp"
#include "fileio/resmgr/res_image.hpp"
#include "fileio/resmgr/res_font.hpp"
#include "spell.hpp"
#include "tools/enum_map.hpp"
#include "tools/winutil.hpp"
#include "replay.hpp"
#include <boost/lexical_cast.hpp>

typedef struct {
	std::string message;
	int line_count;
	int duplicate_count;
	bool duplicate_overflow;
} buf_msg;

buf_msg text_buffer[TEXT_BUF_LEN];

short buf_pointer = 30;

rectangle status_panel_clip_rect = {11, 299, 175, 495},item_panel_clip_rect = {11,297,175,463};

rectangle item_buttons_from[7] = {
	{12,0,24,14},{12,14,24,28},{12,28,24,42},{12,42,24,56},
	{24,0,36,30},{24,30,36,60},{36,0,48,30}
};

eGameMode store_mode;

extern eStatMode stat_screen_mode;

// graphics globals
extern short which_combat_type;
extern eGameMode overall_mode;
extern eItemWinMode stat_window;
extern rectangle more_info_button;
extern short which_item_page[6];
extern std::shared_ptr<cScrollbar> text_sbar,item_sbar;
extern location store_anim_ul;
extern tessel_ref_t bg[];
extern short dest_personalities[40];
extern location source_locs[6];
extern location dest_locs[40];
extern location center;

extern cCustomGraphics spec_scen_g;

// game globals
extern enum_map(eItemButton, rectangle) item_buttons[8];
extern enum_map(ePlayerButton, rectangle) pc_buttons[6];
extern enum_map(eItemButton, bool) item_area_button_active[8];
extern enum_map(ePlayerButton, bool) pc_area_button_active[6];
extern rectangle item_screen_button_rects[9];
extern std::vector<int> spec_item_array;
extern bool prime_time();
// combat globals
extern short item_bottom_button_active[9];
extern cUniverse univ;
extern short shop_identify_cost, shop_recharge_limit;
extern short store_selling_values[8];
extern short combat_posing_monster, current_working_monster; // 0-5 PC 100 + x - monster x
extern bool supressing_some_spaces;
extern location ok_space[4];

// Draws the pc area in upper right
void put_pc_screen() {
	rectangle erase_rect = {17,2,98,269},to_draw_rect,from_rect;
	rectangle food_rect[2] = {{103,34,114,76}, {103,3,114,40}};
	rectangle gold_rect[2] = {{103,106,114,147}, {103,75,114,104}};
	rectangle day_rect[2] = {{103,174,114,201}, {103,147,114,172}};
	rectangle title_rects[3] = {{4,4,16,180}, {4,184,16,214}, {4,214,16,237}};
	rectangle bottom_bar_rect = {99,0,116,271};
	rectangle info_from = {0,1,12,13}, switch_from = {0, 13, 12, 25};
	
	pc_stats_gworld().setActive(false);
	clear_scale_aware_text(pc_stats_gworld());
	
	// First clean up gworld with pretty patterns
	sf::Texture& orig = *ResMgr::graphics.get("statarea");
	rect_draw_some_item(orig, rectangle(orig), pc_stats_gworld(), rectangle(pc_stats_gworld()));
	tileImage(pc_stats_gworld(), erase_rect,bg[6]);
	
	TextStyle style;
	style.font = FONT_BOLD;
	style.pointSize = 10;
	style.colour = Colours::YELLOW;
	style.lineHeight = 10;
	win_draw_string(pc_stats_gworld(),food_rect[1],"Food:",eTextMode::WRAP,style);
	win_draw_string(pc_stats_gworld(),gold_rect[1],"Gold:",eTextMode::WRAP,style);
	win_draw_string(pc_stats_gworld(),day_rect[1],"Day:",eTextMode::WRAP,style);
	win_draw_string(pc_stats_gworld(),title_rects[0],"Party stats:",eTextMode::WRAP,style);
	win_draw_string(pc_stats_gworld(),title_rects[1],"HP:",eTextMode::WRAP,style);
	win_draw_string(pc_stats_gworld(),title_rects[2],"SP:",eTextMode::WRAP,style);
	
	style.colour = Colours::WHITE;
	style.pointSize = 12;
	// Put food, gold, day
	win_draw_string(pc_stats_gworld(),food_rect[0],std::to_string(univ.party.food),eTextMode::WRAP,style);
	win_draw_string(pc_stats_gworld(),gold_rect[0],std::to_string(univ.party.gold),eTextMode::WRAP,style);
	win_draw_string(pc_stats_gworld(),day_rect[0],std::to_string(univ.party.calc_day()),eTextMode::WRAP,style);
	style.colour = Colours::BLACK;
	
	sf::Texture& invenbtn_gworld = *ResMgr::graphics.get("invenbtns");
	for(short i = 0; i < 6; i++) {
		if(univ.party[i].main_status != eMainStatus::ABSENT) {
			for(auto& flag : pc_area_button_active[i])
				flag = true;
			if(i == univ.cur_pc) {
				style.italic = true;
				style.colour = Colours::BLUE;
			}
			
			std::ostringstream sout;
			sout << i + 1 << ". " << univ.party[i].name;
			win_draw_string(pc_stats_gworld(),pc_buttons[i][PCBTN_NAME],sout.str(),eTextMode::ELLIPSIS,style);
			style.italic = false;
			style.colour = Colours::BLACK;
			
			to_draw_rect = pc_buttons[i][PCBTN_HP];
			to_draw_rect.right += 20;
			clear_sstr(sout);
			switch(univ.party[i].main_status) {
				case eMainStatus::ALIVE:
					if(univ.party[i].cur_health == univ.party[i].max_health)
						style.colour = Colours::GREEN;
					else if(univ.party[i].cur_health > univ.party[i].max_health)
						style.colour = Colours::ORANGE;
					else style.colour = Colours::RED;
					win_draw_string( pc_stats_gworld(),pc_buttons[i][PCBTN_HP],std::to_string(univ.party[i].cur_health),eTextMode::WRAP,style);
					if(univ.party[i].cur_sp == univ.party[i].max_sp)
						style.colour = Colours::BLUE;
					else if(univ.party[i].cur_sp > univ.party[i].max_sp)
						style.colour = Colours::TEAL;
					else style.colour = Colours::PINK;
					win_draw_string( pc_stats_gworld(),pc_buttons[i][PCBTN_SP],std::to_string(univ.party[i].cur_sp),eTextMode::WRAP,style);
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
				win_draw_string( pc_stats_gworld(),to_draw_rect,sout.str(),eTextMode::WRAP,style);
			style.colour = Colours::BLACK;
			
			// Now put trade and info buttons
			rect_draw_some_item(invenbtn_gworld,info_from,pc_stats_gworld(),pc_buttons[i][PCBTN_INFO],sf::BlendAlpha);
			rect_draw_some_item(invenbtn_gworld,switch_from,pc_stats_gworld(),pc_buttons[i][PCBTN_TRADE],sf::BlendAlpha);
		}
		else {
			for(auto& flag : pc_area_button_active[i])
				flag = false;
		}
	}
	
	rectangle help_from_rect = {46,60,59,76};
	to_draw_rect = {101,251,114,267};
	rect_draw_some_item(invenbtn_gworld, help_from_rect, pc_stats_gworld(), to_draw_rect, sf::BlendAlpha);
	
	pc_stats_gworld().setActive();
	pc_stats_gworld().display();
	
	// Sometimes this gets called when character is slain. when that happens, if items for
	// that PC are up, switch item page.
	if(univ.cur_pc < 6 && univ.current_pc().main_status != eMainStatus::ALIVE && stat_window == univ.cur_pc) {
		set_stat_window_for_pc(univ.cur_pc);
	}
}

// Draws item area in middle right
// Screen_num is what page is visible in the item menu.
void put_item_screen(eItemWinMode screen_num) {
	std::ostringstream sout;
	long i_num;
	long item_offset;
	short pc;
	rectangle erase_rect = {17,2,122,255},dest_rect;
	rectangle upper_frame_rect = {3,3,15,268};
	
	item_stats_gworld().setActive(false);
	clear_scale_aware_text(item_stats_gworld());
	
	// First clean up gworld with pretty patterns
	sf::Texture& orig = *ResMgr::graphics.get("inventory");
	rect_draw_some_item(orig, rectangle(orig), item_stats_gworld(), rectangle(item_stats_gworld()));
	tileImage(item_stats_gworld(), erase_rect,bg[6]);
	
	// Draw buttons at bottom
	item_offset = item_sbar->getPosition();
	
	for(short i = 0; i < LINES_IN_ITEM_WIN; i++)
		for(auto& flag : item_area_button_active[i])
			flag = false;
	
	TextStyle title_style;
	title_style.lineHeight = 10;
	title_style.font = FONT_BOLD;
	title_style.colour = Colours::YELLOW;
	std::string title = "";
	switch(screen_num) {
		case ITEM_WIN_SPECIAL:
			title = "Special items:";
			break;
		case ITEM_WIN_QUESTS:
			title = "Quests/Jobs:";
			break;
			
		default: // on an items page
			pc = screen_num;
			clear_sstr(sout);
			sout << univ.party[pc].name << " inventory:";
			title = sout.str();
			break;
	}
	win_draw_string(item_stats_gworld(),upper_frame_rect,title,eTextMode::ELLIPSIS,title_style);

	TextStyle line_style;
	auto draw_item_string = [&line_style](int i, std::string str, eFont font, sf::Color colour, bool italic = false, location offset = {0, 0}){
		rectangle dest_rect = item_buttons[i][ITEMBTN_NAME];
		dest_rect.offset(offset);
		line_style.font = font;
		line_style.colour = colour;
		line_style.italic = italic;
		win_draw_string(item_stats_gworld(), dest_rect, str, eTextMode::WRAP, line_style);
	};

	clip_rect(item_stats_gworld(), erase_rect);
	switch(screen_num) {
		case ITEM_WIN_SPECIAL:
			for(short i = 0; i < LINES_IN_ITEM_WIN; i++) {
				i_num = i + item_offset;
				if(i_num < spec_item_array.size()) {
					draw_item_string(i, univ.scenario.special_items[spec_item_array[i_num]].name, FONT_BOLD, Colours::BLACK);
					
					item_area_button_active[i][ITEMBTN_NAME] = true;
					place_item_button(i,ITEMBTN_INFO);
					if((univ.scenario.special_items[spec_item_array[i_num]].flags % 10 == 1)
						&& (!(is_combat()))){
						// Put a Use button where the Drop button normally goes,
						// so there's no gap between Use and Info:
						place_item_button(i,ITEMBTN_USE, ITEMBTN_DROP);
					}
				}
			}
			break;
		case ITEM_WIN_QUESTS:
			for(short i = 0; i < LINES_IN_ITEM_WIN; i++) {
				sf::Color colour = Colours::BLACK;
				i_num = i + item_offset;
				if(i_num < spec_item_array.size()) {
					int which_quest = spec_item_array[i_num] % 10000;
					if(spec_item_array[i_num] / 10000 == 2)
						colour = Colours::RED;
					
					draw_item_string(i, univ.scenario.quests[which_quest].name, FONT_BOLD, colour);
					
					if(spec_item_array[i_num] / 10000 == 1) {
						location from, to;
						from = to = item_buttons[i][ITEMBTN_NAME].centre();
						from.x = item_buttons[i][ITEMBTN_NAME].left;
						to.x = from.x + string_length(univ.scenario.quests[which_quest].name, line_style);
						draw_line(item_stats_gworld(), from, to, 1, Colours::GREEN);
					}
					
					item_area_button_active[i][ITEMBTN_NAME] = true;
					place_item_button(i,ITEMBTN_INFO);
				}
			}
			break;
			
		default: // on an items page			
			for(short i = 0; i < LINES_IN_ITEM_WIN; i++) {
				i_num = i + item_offset;
				clear_sstr(sout);
				sout << i_num + 1 << '.';
				draw_item_string(i, sout.str(), FONT_PLAIN, Colours::BLACK);

 				dest_rect = item_buttons[i][ITEMBTN_NAME];
				dest_rect.left += 36;
				dest_rect.top -= 2;
				
				const cPlayer& who = univ.party[pc];
				const cItem& item = who.items[i_num];

				bool italic = false;
				sf::Color colour = Colours::BLACK;
				if(item.variety != eItemType::NO_ITEM) {
					if(who.equip[i_num]) {
						italic = true;
						if(item.variety == eItemType::ONE_HANDED || item.variety == eItemType::TWO_HANDED)
							colour = Colours::PINK;
						else if((*item.variety).is_armour)
							colour = Colours::GREEN;
						else colour = Colours::BLUE;
					}
					
					clear_sstr(sout);

					if(item.ident)
						sout << item.full_name << ' ';
					else
						sout << item.name << "  ";

					// Charges:
					bool show_charges = item.max_charges > 1 || item.charges > 1; // stacked gems have no max, but do have charges
					// Show charges for unidentified ammunition and lockpicks, but not other unidentified items
					if(item.missile < 0 && item.ability != eItemAbil::LOCKPICKS)
						show_charges &= item.ident;
					// Don't show charges if it just shows a dialog
					show_charges &= item.ability != eItemAbil::MESSAGE;
					// Don't show charges when Sell button up and space tight
					show_charges &=	(stat_screen_mode == MODE_INVEN || stat_screen_mode == MODE_SHOP);
					if(show_charges){
						sout << '(' << int(item.charges) << ')';
					}

					dest_rect.left -= 2;
					draw_item_string(i, sout.str(), FONT_PLAIN, colour, italic, {34, -2});
					
					place_item_graphic(i,item.graphic_num); 
					// The info button is harmless and can be useful while shopping, so always show it
					place_item_button(i,ITEMBTN_INFO);

					if(stat_screen_mode == MODE_INVEN &&
						prime_time() && pc == univ.cur_pc) {
						
						// place give and drop and use
						place_item_button(i,ITEMBTN_GIVE);
						place_item_button(i,ITEMBTN_DROP);
						if(item.can_use() && (item.rechargeable ? item.charges > 0 : true)) // place use if can
							place_item_button(i,ITEMBTN_USE);
					}

					if(stat_screen_mode != MODE_INVEN && stat_screen_mode != MODE_SHOP) {
						place_buy_button(i,pc,i_num);
						
					}
					
				} // end of if item is there
			} // end of for(short i = 0; i < LINES_IN_ITEM_WIN; i++)
			break;
	}
	undo_clip(item_stats_gworld());
	
	place_item_bottom_buttons();
	item_stats_gworld().setActive();
	item_stats_gworld().display();
}

void place_buy_button(short position,short pc_num,short item_num) {
	rectangle dest_rect,source_rect;
	rectangle button_sources[4] = {{24,0,36,30},{36,0,48,30},{48,0,60,30},{60,0,72,30}};
	short val_to_place;
	
	const cPlayer& pc = univ.party[pc_num];
	const cItem& item = pc.items[item_num];
	
	if(item.variety == eItemType::NO_ITEM)
		return;
	
	dest_rect = item_buttons[position][ITEMBTN_SPEC];
	
	val_to_place = (item.charges > 0) ?
		item.charges * item.value :
		item.value;
	val_to_place = val_to_place / 2;
	
	switch(stat_screen_mode) {
		case MODE_IDENTIFY:
			if(!item.ident) {
				item_area_button_active[position][ITEMBTN_SPEC] = true;
				source_rect = button_sources[0];
				val_to_place = shop_identify_cost;
			}
			break;
		case MODE_RECHARGE:
			if(item.rechargeable && (shop_recharge_limit == 0 || item.charges < item.max_charges / shop_recharge_limit) && item.can_use()) {
				item_area_button_active[position][ITEMBTN_SPEC] = true;
				source_rect = button_sources[3];
				val_to_place = shop_identify_cost;
			}
		   break;
		case MODE_SELL_WEAP:
			if((*item.variety).is_weapon && !pc.equip[item_num] && item.ident && val_to_place > 0 && !item.unsellable) {
				item_area_button_active[position][ITEMBTN_SPEC] = true;
				source_rect = button_sources[1];
			}
			break;
		case MODE_SELL_ARMOR:
			if((*item.variety).is_armour && !pc.equip[item_num] && item.ident && val_to_place > 0 && !item.unsellable) {
				item_area_button_active[position][ITEMBTN_SPEC] = true;
				source_rect = button_sources[1];
			}
			break;
		case MODE_SELL_ANY:
			if(!pc.equip[item_num] && item.ident && val_to_place > 0 && !item.unsellable) {
				item_area_button_active[position][ITEMBTN_SPEC] = true;
				source_rect = button_sources[1];
			}
			break;
		case MODE_ENCHANT:
			if((item.variety == eItemType::ONE_HANDED || item.variety == eItemType::TWO_HANDED) && item.ident && item.ability == eItemAbil::NONE && !item.magic) {
				item_area_button_active[position][ITEMBTN_SPEC] = true;
				source_rect = button_sources[2];
				val_to_place = (*eEnchant(shop_identify_cost)).adjust_value(item.value);
			}
			break;
		case MODE_INVEN: case MODE_SHOP:
			// These modes don't have buy buttons, so we shouldn't get here; bail out!
			return;
	}
	if(item_area_button_active[position][ITEMBTN_SPEC]) {
		sf::Texture& invenbtn_gworld = *ResMgr::graphics.get("invenbtns");
		store_selling_values[position] = val_to_place;
		dest_rect = item_buttons[position][ITEMBTN_SPEC];
		dest_rect.right = dest_rect.left + 30;
		rect_draw_some_item(invenbtn_gworld, source_rect, item_stats_gworld(), dest_rect, sf::BlendAlpha);
		TextStyle style;
		if(val_to_place >= 10000)
			style.font = FONT_PLAIN;
		style.lineHeight = 10;
		dest_rect.offset(dest_rect.width() + 5,0);
		win_draw_string(item_stats_gworld(),dest_rect,std::to_string(val_to_place),eTextMode::LEFT_TOP,style);
	}
}

void place_item_graphic(short which_slot,short graphic) {
	rectangle from_rect = {0,0,18,18},to_rect;
	
	item_area_button_active[which_slot][ITEMBTN_NAME] = item_area_button_active[which_slot][ITEMBTN_ICON] = true;
	from_rect.offset((graphic % 10) * 18,(graphic / 10) * 18);
	to_rect = item_buttons[which_slot][ITEMBTN_ICON];
	std::shared_ptr<const sf::Texture> src_gw;
	if(graphic >= 10000) {
		graf_pos_ref(src_gw, from_rect) = spec_scen_g.find_graphic(graphic - 10000, true);
		rect_draw_some_item(*src_gw, from_rect, item_stats_gworld(), to_rect,sf::BlendAlpha);
	} else if(graphic >= 1000) {
		graf_pos_ref(src_gw, from_rect) = spec_scen_g.find_graphic(graphic - 1000);
		rect_draw_some_item(*src_gw, from_rect, item_stats_gworld(), to_rect,sf::BlendAlpha);
	}
	else rect_draw_some_item(*ResMgr::graphics.get("tinyobj"), from_rect, item_stats_gworld(), to_rect, sf::BlendAlpha);
}

// name, use, give, drop, info, sell/id
void place_item_button(short which_slot,eItemButton button_type, eItemButton button_pos) {
	if(button_pos == MAX_eItemButton){
		button_pos = button_type;
	}

	rectangle from_rect = {0,0,18,18},to_rect;

	sf::Texture& invenbtn_gworld = *ResMgr::graphics.get("invenbtns");
	item_area_button_active[which_slot][button_pos] = true;
	rect_draw_some_item(invenbtn_gworld, item_buttons_from[button_type - 2], item_stats_gworld(), item_buttons[which_slot][button_pos], sf::BlendAlpha);
}

void place_item_bottom_buttons() {
	rectangle pc_from_rect = {0,0,36,28},but_from_rect = {30,60,46,78},to_rect;
	rectangle spec_from_rect = {0,60,15,95}, job_from_rect = {15,60,30,95}, help_from_rect = {46,60,59,76};
	// TODO: What about when the buttons are pressed?
	TextStyle style;
	style.lineHeight = 10;
	style.pointSize = 12;
	style.font = FONT_BOLD;
	style.colour = Colours::YELLOW;
	
	sf::Texture& invenbtn_gworld = *ResMgr::graphics.get("invenbtns");
	for(short i = 0; i < 6; i++) {
		if(univ.party[i].main_status == eMainStatus::ALIVE) {
		 	item_bottom_button_active[i] = true;
		 	to_rect = item_screen_button_rects[i];
			rect_draw_some_item(invenbtn_gworld, but_from_rect, item_stats_gworld(), to_rect, sf::BlendAlpha);
			pic_num_t pic = univ.party[i].which_graphic;
			std::shared_ptr<const sf::Texture> from_gw;
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
				from_gw = &ResMgr::graphics.get("monst" + std::to_string(1 + which_sheet));
			} else {
				pc_from_rect = calc_rect(2 * (pic / 8), pic % 8);
				from_gw = &ResMgr::graphics.get("pcs");
			}
			to_rect.inset(2,2);
			rect_draw_some_item(*from_gw, pc_from_rect, item_stats_gworld(), to_rect, sf::BlendAlpha);
			std::string numeral = std::to_string(i + 1);
			short width = string_length(numeral, style);
			// Offset "6" down an extra pixel to make it line up, because it has an ascender in this font
			to_rect.offset(-width - 5, i == 5 ? 3 : 2);
			win_draw_string(item_stats_gworld(), to_rect, numeral, eTextMode::LEFT_TOP, style);
		}
		else item_bottom_button_active[i] = false;
	}
	to_rect = item_screen_button_rects[6];
	rect_draw_some_item(invenbtn_gworld, spec_from_rect, item_stats_gworld(), to_rect, sf::BlendAlpha);

	// Don't draw the Jobs button if the scenario has none
	item_bottom_button_active[7] = false;
	if(!univ.scenario.quests.empty()){
		item_bottom_button_active[7] = true;
		to_rect = item_screen_button_rects[7];
		rect_draw_some_item(invenbtn_gworld, job_from_rect, item_stats_gworld(), to_rect, sf::BlendAlpha);
	}

	to_rect = item_screen_button_rects[8];
	rect_draw_some_item(invenbtn_gworld, help_from_rect, item_stats_gworld(), to_rect, sf::BlendAlpha);
}

void set_stat_window_for_pc(int pc, bool record) {
	if(pc < 0) pc = 0;
	if(pc > 5) pc = 5;
	set_stat_window(eItemWinMode(pc), record);
}

void set_stat_window(eItemWinMode new_stat, bool record) {
	if(record && recording){
		record_action("set_stat_window", boost::lexical_cast<std::string>((int)new_stat));
	}
	if(new_stat == ITEM_WIN_SPECIAL)
		give_help(50,0);
	
	short array_pos = 0;
	
	stat_window = new_stat;
	if(stat_window < ITEM_WIN_SPECIAL && univ.party[stat_window].main_status != eMainStatus::ALIVE)
		stat_window = eItemWinMode(first_active_pc());
	item_sbar->setPageSize(LINES_IN_ITEM_WIN);
	spec_item_array.clear();
	switch(stat_window) {
		case ITEM_WIN_SPECIAL:
			std::fill(spec_item_array.begin(), spec_item_array.end(), -1);
			for(short i = 0; i < univ.scenario.special_items.size(); i++)
				if(univ.party.spec_items.count(i)) {
					spec_item_array.push_back(i);
					array_pos++;
				}
			array_pos = max(0,array_pos - LINES_IN_ITEM_WIN);
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
			array_pos = max(0,array_pos - LINES_IN_ITEM_WIN);
			item_sbar->setMaximum(array_pos);
			break;
		default:
			item_sbar->setMaximum(cPlayer::INVENTORY_SIZE - LINES_IN_ITEM_WIN);
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
	extern enum_map(eGuiArea, rectangle) win_to_rects;
	
	if(mode == 1) x = sf::BlendAdd;
	else x = sf::BlendNone;
	rect_draw_some_item(pc_stats_gworld(), rectangle(pc_stats_gworld()), mainPtr(), win_to_rects[WINRECT_PCSTATS], x);
	rect_draw_some_item(item_stats_gworld(), rectangle(item_stats_gworld()), mainPtr(), win_to_rects[WINRECT_INVEN], x);
	item_sbar->draw();
	if(mode != 1){
		rect_draw_some_item(text_area_gworld(), rectangle(text_area_gworld()), mainPtr(), win_to_rects[WINRECT_TRANSCRIPT], x);
		text_sbar->draw();
	}
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
	right_limit = pc_buttons[0][PCBTN_HP].left - 5;
	dest_rect.left = name_width + 33;
	dest_rect.right = dest_rect.left + 12;
	dest_rect.top += pc * 13;
	dest_rect.bottom += pc * 13;
	
	if(exceptSplit(univ.party[pc].main_status) != eMainStatus::ALIVE)
		return;
	
	univ.party[pc].status[eStatus::HASTE_SLOW]; // This just makes sure it exists in the map, without changing its value if it does
	sf::Texture& status_gworld = *ResMgr::graphics.get("staticons");
	for(auto next : univ.party[pc].status) {
		short placedIcon = -1;
		const auto& statInfo = *next.first;
		if(statInfo.special && next.second >= statInfo.special->lo && next.second <= statInfo.special->hi) {
			placedIcon = statInfo.special->icon;
		}
		else if(next.second > 0) placedIcon = statInfo.icon;
		else if(next.second < 0) placedIcon = statInfo.negIcon;
		if(placedIcon >= 0) {
			rect_draw_some_item(status_gworld, get_stat_effect_rect(placedIcon), pc_stats_gworld(), dest_rect, sf::BlendAlpha);
			dest_rect.offset(13, 0);
		}
		if(dest_rect.right >= right_limit) break;
	}
}


void print_party_stats() {
	if(recording){
		record_action("print_party_stats", "");
	}
	if(overall_mode == MODE_STARTUP) return;
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
			if((univ.town.monst[i].is_alive()) && (is_lit)
				&& univ.town.monst[i].on_space(space) &&
				((overall_mode == MODE_LOOK_TOWN) || (can_see_light(univ.current_pc().combat_pos,space,sight_obscurity) < 5))
				&& (univ.town.monst[i].picture_num != 0)) {
				
				
				msg = "    ";
				if(univ.town.monst[i].health < univ.town.monst[i].m_health)
					msg += "Wounded ";
				msg += get_m_name(univ.town.monst[i].number);
				msg += univ.town.monst[i].is_friendly() ? " (F)" : " (H)";
				
				add_string_to_buf(msg);
				
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
						add_string_to_buf(msg);
						j = 7;
						
					}
				
			}
		}
		location lSpace=global_to_local(space);
		if(univ.out->roads[lSpace.x][lSpace.y])
			add_string_to_buf("    Road");
		if(out_boat_there(space))
			add_string_to_buf("    Boat");
		if(out_horse_there(space))
			add_string_to_buf("    Horse");
		if(univ.out->special_spot[lSpace.x][lSpace.y])
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
				add_string_to_buf(msg);
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
	location sector = {univ.party.outdoor_corner.x + univ.party.i_w_c.x, univ.party.outdoor_corner.y + univ.party.i_w_c.y};
	for(short i = 0; i < univ.party.boats.size(); i++)
		if((univ.party.boats[i].exists) && (where == univ.party.boats[i].loc)
			&& (univ.party.boats[i].which_town == 200) && (sector == univ.party.boats[i].sector))
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

static void print_monster_count(std::string m_name, short num){
	if(num > 0){
		std::ostringstream sout;
		sout << "  ";
		if(num > 1){
			sout << num << " x ";
		}
		sout << m_name;
		add_string_to_buf(sout.str());
	}
}

void print_encounter_monsters(cOutdoors::cWandering encounter, short* nums, std::map<std::string,short> alive) {
	std::string m_name;
	bool remaining_specified = !alive.empty();
	// Still follow the predefined ordering when giving an updated total, and leave summons to the end.
	for(short i = 0; i < 7; i++)
		if(encounter.monst[i] != 0) {
			m_name = get_m_name(encounter.monst[i]);
			short num = 0;
			// Combat is just starting, so print the predefined amount
			if(!remaining_specified){
				num = nums[i];
			}
			// Combat has started, so need to check how many are still alive
			else{
				auto iter = alive.find(m_name);
				if(iter != alive.end()){
					num = iter->second;
					alive.erase(iter);
				}
			}
			print_monster_count(m_name, num);
		}
	// Monsters still in the alive map must have been summoned
	if(!alive.empty()){
		for (auto iter = alive.begin(); iter != alive.end(); ++iter){
			print_monster_count(iter->first, iter->second);
		}
	}
}

void notify_out_combat_began(cOutdoors::cWandering encounter,short *nums) {
	add_string_to_buf("COMBAT!");
	print_encounter_monsters(encounter, nums);
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
	add_string_to_buf(msg);
}

void damaged_message(short damage,eMonstMelee type) {
	std::ostringstream sout;
	sout << "    " << get_str("monster-abilities",130 + int(type));
	sout << " for " << damage;
	add_string_to_buf(sout.str());
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
	add_string_to_buf(sout.str());
	
}

short print_terrain(location space) {
	ter_num_t which_terrain = 0;
	
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
	size_t starting_indent = str.find_first_not_of(' ');
	if(starting_indent != std::string::npos)
		add_string_to_buf(str, starting_indent + 2);
}

void add_string_to_buf(std::string str, unsigned short indent) {
	if(overall_mode == MODE_STARTUP)
		return;
	if(str == "") return;
	if(str.find_last_not_of(' ') == std::string::npos)
		return;

	if(indent > 20) indent = 20;

	static bool inited;
	static size_t width;
	static TextStyle buf_style;
	if(!inited) {
		inited = true;
		buf_style.font = FONT_PLAIN;
		buf_style.pointSize = 12;
		width = text_area_gworld().getSize().x - 5;
	}

	std::string wrapped_str = str;
	int line_count = 1 + std::count(str.begin(), str.end(), '\n');
	int wrapped_idx = 0;
	std::string space(indent, ' ');
	while(string_length(wrapped_str.substr(wrapped_idx, wrapped_str.find_last_not_of(' ')), buf_style) >= width) {
		size_t split = wrapped_str.find_last_of(' ');
		while(string_length(wrapped_str.substr(wrapped_idx, split), buf_style) >= width)
			split = wrapped_str.find_last_of(' ', split - 1);

		wrapped_str[split] = '\n';
		wrapped_str.insert(split + 1, space);

		wrapped_idx = split + 1 + indent;
		line_count += 1;
	}
	
	// Check if this is a duplicate message
	int prev_pointer = buf_pointer - 1;
	if(prev_pointer < 0) prev_pointer = TEXT_BUF_LEN - 1;

	if(wrapped_str == text_buffer[prev_pointer].message){
		text_buffer[prev_pointer].duplicate_count++;
		std::string test_fit = wrapped_str + " (x" + std::to_string(text_buffer[prev_pointer].duplicate_count) + ")";
		if(string_length(test_fit, buf_style) >= width){
			text_buffer[prev_pointer].line_count = line_count + 1;
			// The duplicate count in parenthesis is on its own line
			text_buffer[prev_pointer].duplicate_overflow = true;
		}
	}else{
		text_buffer[buf_pointer].message = wrapped_str;
		text_buffer[buf_pointer].line_count = line_count;
		text_buffer[buf_pointer].duplicate_count = 1;
		text_buffer[buf_pointer].duplicate_overflow = false;
		if(buf_pointer == (TEXT_BUF_LEN - 1))
			buf_pointer = 0;
		else buf_pointer++;
	}
	
	text_sbar->setPosition(58); // TODO: This seems oddly specific
}

void add_caster_needs_to_buf(std::string needs_what, unsigned short pre_indent, unsigned short indent) {
	std::ostringstream sout;
	for(int i = 0; i < pre_indent; ++i){
		sout << ' ';
	}
	extern short pc_casting;
	sout << univ.party[pc_casting].name << " needs " << needs_what << ".";
	add_string_to_buf(sout.str(), indent);
}

void init_buf() {
	for(short i = 0; i < TEXT_BUF_LEN; i++){
		// Buffer messages are no longer forced down to 50 chars each. In an effort to keep memory usage predictable,
		// I've set an expected capacity of 100 per message.
		text_buffer[i].message.reserve(100);
		text_buffer[i].line_count = 1;
		text_buffer[i].duplicate_count = 1;
		text_buffer[i].duplicate_overflow = false;
	}
}

void print_buf () {
	rectangle store_text_rect,dest_rect,erase_rect = {2,2,136,255};
	
	text_area_gworld().setActive(false);
	clear_scale_aware_text(text_area_gworld());
	
	// First clean up gworld with pretty patterns
	tileImage(text_area_gworld(), erase_rect,bg[6]);

	// Don't draw wrapped scale-aware text outside of the viewport
	clip_rect(text_area_gworld(), erase_rect);
	
	// Handling scrolling is a lot more confusing now.
	// Think of line 0 as the LAST line of the most recently printed message. We count up from that accounting for
	// multiple lines being stored in each message, to find the BOTTOM-MOST message to print first.
	long lines_clipped_below = 58 - text_sbar->getPosition();
	int message_idx = buf_pointer - 1;
	if(message_idx < 0)
		message_idx = TEXT_BUF_LEN + message_idx;
	int line = 0;
	while(line + text_buffer[message_idx].line_count <= lines_clipped_below){
		line += text_buffer[message_idx].line_count;
		message_idx--;
		if(message_idx < 0)
			message_idx = TEXT_BUF_LEN + message_idx;
	}
	
	// Shift the text down this many lines, counting on clip_rect to hide the overflow:
	int line_offset = lines_clipped_below - line;
	int num_lines_total = 0;
	int num_lines_visible = -line_offset;
	
	location moveTo;
	TextStyle line_style;
	line_style.font = FONT_PLAIN;
	line_style.colour = Colours::BLACK;
	line_style.pointSize = 12;

	while(num_lines_visible < LINES_IN_TEXT_WIN){
		std::string message = text_buffer[message_idx].message;
		int indent = message.find_first_not_of(' ');
		if(indent != std::string::npos){
			if(text_buffer[message_idx].duplicate_count > 1){
				if(text_buffer[message_idx].duplicate_overflow){
					message += "\n" + std::string(indent, ' ');
				}else{
					message += " ";
				}
				message += "(x" + std::to_string(text_buffer[message_idx].duplicate_count) + ")";
			}

			moveTo = location(4, 1 + 12 * (LINES_IN_TEXT_WIN + line_offset - num_lines_total - text_buffer[message_idx].line_count));
			sf::Text text;
			line_style.applyTo(text, get_ui_scale());
			// A spacing factor of 1.0 within multiline messages doesn't actually line up with other single buffer lines
			text.setLineSpacing(0.85);
			text.setString(message);
			text.setPosition(moveTo);
			draw_scale_aware_text(text_area_gworld(), text);
		}
		
		num_lines_visible += text_buffer[message_idx].line_count;
		num_lines_total += text_buffer[message_idx].line_count;
		message_idx--;
		if(message_idx < 0)
			message_idx = TEXT_BUF_LEN + message_idx;
	}
	
	undo_clip(text_area_gworld());
	text_area_gworld().setActive();
	text_area_gworld().display();
}

void restore_mode() {
	overall_mode = store_mode;
}

/* Draw a bitmap in the world window. hor in 0 .. 8, vert in 0 .. 8,
 object is ptr. to bitmap to be drawn, and masking is for Copybits. */
void Draw_Some_Item(const sf::Texture& src_gworld, rectangle src_rect, sf::RenderTarget& targ_gworld,location target, char masked, short main_win) {
	rectangle	destrec = {0,0,36,28};
	
	if((target.x < 0) || (target.y < 0) || (target.x > 8) || (target.y > 8))
		return;
	if((supressing_some_spaces) && (target != ok_space[0]) &&
		(target != ok_space[1]) && (target != ok_space[2]) && (target != ok_space[3]))
		return;
	
	destrec = coord_to_rect(target.x,target.y);
	if(main_win == 1) destrec.offset(5,5);
	
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
	style.colour = Colours::WHITE;
	rectangle back_rect = label.text_rect, text_rect = label.text_rect;
	back_rect.inset(-7,-7);
	back_rect.offset(0,-2);
	for(int i = 0; i <= 3; i++) {
		fill_roundrect(terrain_screen_gworld(), back_rect, 7, back_clr);
		back_rect.inset(2,2);
		back_clr.a *= 1.5;
	}
	//text_rect.offset(0, -text_rect.height() + 1);
	text_rect.offset(0, -5);
	win_draw_string(terrain_screen_gworld(), text_rect, label.str, eTextMode::LEFT_TOP, style);
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

std::string get_location(cUniverse* specific_univ) {
	// This function is used to determine text bar text, which may be intended to be blank
	// even if the party is technically outdoors, depending on the active game mode.
	// I'm just trying to keep the old behavior the same.
	bool outdoors = is_out();
	bool town = is_town();
	// For checking a save file's location, it can only be one or the other.
	if(specific_univ != nullptr){
		outdoors = specific_univ->party.town_num >= 200;
		town = !outdoors;
	}else{
		specific_univ = &univ;
	}

	std::string loc_str = "";

	location loc = outdoors ? global_to_local(specific_univ->party.out_loc) : specific_univ->party.town_loc;
	if(outdoors) {
		loc_str = specific_univ->out->name;
		for(short i = 0; i < specific_univ->out->area_desc.size(); i++)
			if(loc.in(specific_univ->out->area_desc[i])) {
				loc_str = specific_univ->out->area_desc[i].descr;
			}
	}
	if(town){
		loc_str = specific_univ->town->name;
		for(short i = 0; i < specific_univ->town->area_desc.size(); i++)
			if(loc.in(specific_univ->town->area_desc[i])) {
				loc_str = specific_univ->town->area_desc[i].descr;
			}
	}
	return loc_str;
}
