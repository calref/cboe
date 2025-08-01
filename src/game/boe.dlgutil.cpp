
#include <cstring>
#include <algorithm>
#include <iomanip>
#include <string>
#include <boost/algorithm/string/replace.hpp>

#include "boe.global.hpp"

#include "universe/universe.hpp"

#include "boe.dlgutil.hpp"
#include "boe.text.hpp"
#include "boe.town.hpp"
#include "boe.locutils.hpp"
#include "boe.party.hpp"
#include "boe.specials.hpp"
#include "boe.fileio.hpp"
#include "boe.graphics.hpp"
#include "boe.townspec.hpp"
#include "boe.main.hpp"
#include "boe.items.hpp"
#include "boe.consts.hpp"
#include "sounds.hpp"
#include <cstdio>
#include "boe.newgraph.hpp"
#include "boe.infodlg.hpp"
#include "utility.hpp"
#include "mathutil.hpp"
#include "dialogxml/dialogs/strdlog.hpp"
#include "dialogxml/dialogs/choicedlog.hpp"
#include "gfx/render_shapes.hpp"
#include "tools/winutil.hpp"
#include "fileio/fileio.hpp"
#include "fileio/resmgr/res_dialog.hpp"
#include "fileio/resmgr/res_strings.hpp"
#include "dialogxml/widgets/field.hpp"
#include "dialogxml/widgets/scrollbar.hpp"
#include "dialogxml/widgets/button.hpp"
#include "dialogxml/widgets/ledgroup.hpp"
#include "dialogxml/widgets/pict.hpp"
#include "dialogxml/widgets/stack.hpp"
#include <boost/lexical_cast.hpp>
#include "tools/prefs.hpp"
#include "scenario/shop.hpp"
#include "tools/cursors.hpp"
#include "tools/enum_map.hpp"
#include "replay.hpp"

extern eItemWinMode stat_window;
extern eGameMode overall_mode;
extern bool changed_display_mode;
extern rectangle d_rects[80];
extern short d_rect_index[80];
extern eStatMode stat_screen_mode;
extern long register_flag;
extern long ed_flag,ed_key;
extern location center;
extern std::shared_ptr<cScrollbar> text_sbar,item_sbar,shop_sbar;
extern std::shared_ptr<cButton> done_btn, help_btn;
extern bool map_visible;
extern cUniverse univ;
extern std::map<eSkill,short> skill_max;
extern void give_help_and_record(short help1, short help2, bool help_forced = false);

short sign_mode,person_graphic,store_person_graphic,store_sign_mode;
long num_talk_entries;
char null_string[256] = "";
extern bool party_in_memory;

// Talking vars
eGameMode store_pre_talk_mode;
short store_personality,store_personality_graphic,shop_identify_cost,shop_recharge_amount,shop_recharge_limit;
std::string save_talk_str1, save_talk_str2;
bool talk_end_forced;
rectangle talk_area_rect = {7,19,422,298}, word_place_rect = {44,7,372,257},talk_help_rect = {7,268,23,286};
std::string title_string;
mon_num_t store_monst_type;
short store_m_num;
rectangle dummy_rect = {0,0,0,0};
bool can_save_talk;
short store_talk_face_pic;
int current_talk_node;
extern std::vector<word_rect_t> talk_words;

// "Go Back" stack: When pressing the "Go Back" button, we don't just want to reverse through
// the nodes the player has seen, because they might change game state the first time, causing
// you to go backwards to a node that would behave differently than it did before.
// The "Go Back" stack allows you to reverse through exactly what the dialog engine has showed you,
// in a read-only fashion as long as you only go backwards.
struct dialog_history_t {
	bool can_record;

	bool special_node;
	// Store node numbers for shop nodes.
	int node;
	// Store text for normal dialog nodes.
	std::pair<std::string, std::string> text;
};
std::vector<dialog_history_t> dialog_history;

// Shopping vars

// 1 - 499 ... regular items
// 500 alchemy
// 600-620 ... food not used!!!
// 700+ i - that healing
// 800 + - mage spells
// 900 + - priest spells
// n000 + i - magic store n item i
// talk_area_rect and talk_help_rect used for this too
eGameMode store_pre_shop_mode;
extern enum_map(eShopArea, rectangle) shopping_rects[8];
rectangle bottom_help_rects[4] = {{356,6,368,250},{374,6,386,270},{386,6,398,250},{398,6,410,250}};
rectangle shop_name_str = {44,6,56,200};
rectangle shop_frame = {62,10,352,269};
rectangle shop_done_rect = {388,212,411,275};
std::vector<int> shop_array;

cShop active_shop;
short active_shop_num;
short store_cur_pc = -1;

extern void init_inven_rects();
extern void init_shopping_rects(bool scrollbar);

// For healing shops, other PCs might be able to buy something if
// the active PC can't
static bool start_shop_mode_other_pc(bool allow_empty = false, bool already_started = false) {
	// The shop might change the current PC multiple times, but we want to restore
	// it to the original active PC when shopping ends, so only store if we're
	// not yet storing
	if(store_cur_pc == -1)
		store_cur_pc = univ.cur_pc;

	// But I actually want to store the PC that's active now, so if no one can buy anything but
	// we want to leave an empty shop, we can leave the PC selection where it is.
	short pc_buying = univ.cur_pc;

	bool other_pc_can_buy = false;
	for(int i = 0; i < 6; ++i){
		if(univ.party[i].main_status != eMainStatus::ABSENT){
			univ.cur_pc = i;
			if(start_shop_mode(active_shop_num,active_shop.getCostAdjust(),save_talk_str1,true,already_started)){
				other_pc_can_buy = true;
				break;
			}
		}
	}
	if(!other_pc_can_buy && allow_empty){
		univ.cur_pc = pc_buying;
		start_shop_mode(active_shop_num,active_shop.getCostAdjust(),save_talk_str1,false,already_started);
	}
	return other_pc_can_buy;
}

bool start_shop_mode(short which,short cost_adj,std::string store_name, bool cancel_when_empty, bool already_started) {
	rectangle area_rect;
	if(which < 0 || which >= univ.scenario.shops.size()) {
		showError("The scenario tried to place you in a nonexistent shop!");
		return false;
	}
	
	// This would be a place to hide the text box, if I add it.
	
	active_shop_num = which;
	active_shop = univ.scenario.shops[which];
	active_shop.setCostAdjust(cost_adj);
	active_shop.setName(store_name);
	
	// Check if the shop's stock has been reduced yet
	if(univ.party.store_limited_stock.find(active_shop_num) != univ.party.store_limited_stock.end()) {
		for(auto p : univ.party.store_limited_stock[active_shop_num]) {
			int which_item, quant_left;
			std::tie(which_item, quant_left) = p;
			if(which_item < 0 || which_item >= active_shop.size()) continue;
			cShopItem entry = active_shop.getItem(which_item);
			if(entry.quantity == 0) continue; // Just in case a stray entry accidentally gets put in for an infinite stock item (or an item was changed from finite to infinite)
			if(quant_left == 0)
				entry.type = eShopItemType::EMPTY;
			else if(entry.type == eShopItemType::OPT_ITEM)
				entry.quantity = quant_left + (entry.quantity / 1000) * 1000;
			else entry.quantity = quant_left;
			active_shop.replaceItem(which_item, entry);
		}
	}
	
	set_up_shop_array();
	if(shop_array.empty() && cancel_when_empty){
		return false;
	}

	area_rect = talk_area_rect;
	talk_gworld().create(area_rect.width(), area_rect.height());

	if(!already_started)
		store_pre_shop_mode = overall_mode;
	overall_mode = MODE_SHOPPING;
	stat_screen_mode = MODE_SHOP;
	shop_sbar->setPosition(0);

	put_background();
	
	init_shopping_rects(shop_sbar->getMaximum() > 0);
	draw_shop_graphics(false,false,area_rect);
	
	put_item_screen(stat_window);
	give_help(26,27);
	return true;
}

// This is for the legacy "Go Back" button behavior, and for some reason doesn't always work:
static void update_last_talk(int new_node) {
	if(has_feature_flag("talk-go-back", "StackV1")) return;
	// Store last node in the Go Back button
	for(word_rect_t& word : talk_words) {
		if(word.word != "Go Back") continue;
		word.node = current_talk_node;
		current_talk_node = new_node;
		break;
	}
}

void end_shop_mode() {
	if(recording){
		record_action("end_shop_mode", "");
	}

	if(store_cur_pc >= 0){
		univ.cur_pc = store_cur_pc;
		store_cur_pc = -1;
	}

	rectangle dummy_rect = {0,0,0,0};
	
	// This would be a place to show the text box, if I add it (and if this is not an outdoor shop).
	
	shop_sbar->hide();
	done_btn->hide();
	if(store_pre_shop_mode == MODE_TALKING) {
		save_talk_str1 = "You conclude your business.";
		save_talk_str2 = "";
		can_save_talk = false;
		place_talk_str(save_talk_str1, save_talk_str2, 0, dummy_rect);
		update_last_talk(TALK_BUSINESS);
	}else{
		help_btn->hide();
	}
	
	overall_mode = store_pre_shop_mode;
	if(overall_mode == MODE_TALK_TOWN)
		overall_mode = MODE_TOWN;
	if(overall_mode == MODE_TOWN) {
		center = univ.party.town_loc;
		update_explored(center);
	}
	stat_screen_mode = MODE_INVEN;
	put_item_screen(stat_window);
	put_pc_screen();
	// TODO: I suspect REFRESH_NONE will suffice here
	redraw_screen(REFRESH_TERRAIN | REFRESH_BAR);
	
	// If it was a random shop, we need to update the stored list of items so that bought items don't reappear
	if(active_shop.getType() == eShopType::RANDOM) {
		auto& this_shop = univ.party.magic_store_items[active_shop_num];
		this_shop.clear();
		for(int i = 0; i < active_shop.size(); i++) {
			cShopItem item = active_shop.getItem(i);
			if(item.type == eShopItemType::TREASURE || item.type == eShopItemType::CLASS || item.type == eShopItemType::OPT_ITEM)
				this_shop[i] = item.item;
		}
	}
	// We also need to save any limited stock
	for(int i = 0; i < active_shop.size(); i++) {
		cShopItem item = active_shop.getItem(i);
		if(item.quantity > 0) {
			// This means the stock is limited.
			int left = item.type == eShopItemType::EMPTY ? 0 : item.quantity;
			univ.party.store_limited_stock[active_shop_num][i] = left;
		}
	}
	init_inven_rects();
}

bool handle_shop_event(location p, cFramerateLimiter& fps_limiter) {
	if(p.in(talk_help_rect)) {
		if(help_btn->handleClick(p, fps_limiter))
			give_help_and_record(226,27);
		return true;
	}
	
	if(p.in(shop_done_rect)) {
		if(done_btn->handleClick(p, fps_limiter))
			end_shop_mode();
		return true;
	}

	for(short i = 0; i < 8; i++) {
		auto shop_i = i + shop_sbar->getPosition();
		if(shop_i >= shop_array.size()) break;
		unsigned long what_picked = shop_array[shop_i];
		if(what_picked >= active_shop.size()) break;
		if(active_shop.getItem(what_picked).type == eShopItemType::EMPTY)
			break;

		// Since shop UI was drawn into the game window with offsets, we need to apply
		// the same offsets to event catching areas.
		rectangle active_rect { shopping_rects[i][SHOPRECT_ACTIVE_AREA] };
		rectangle visual_rect { active_rect };
		visual_rect.right += 10;
		active_rect.offset(talk_gword_offset_x, talk_gword_offset_y);

		rectangle item_help_rect { shopping_rects[i][SHOPRECT_ITEM_HELP] };
		rectangle visual_item_help_rect { item_help_rect };
		item_help_rect.offset(talk_gword_offset_x, talk_gword_offset_y);

		if(p.in(active_rect)) {
			click_shop_rect(visual_rect, false);
			handle_sale(what_picked);
			return true;
		} else if(p.in(item_help_rect)){
			click_shop_rect(visual_item_help_rect, true);
			handle_info_request(what_picked);
			return true;
		}
	}
	return p.in(rectangle(talk_gworld()));
}

void handle_sale(int i) {
	if(recording){
		record_action("handle_sale", boost::lexical_cast<std::string>(i));
	}
	cShopItem item = active_shop.getItem(i);
	cItem base_item = item.item;
	short cost = item.getCost(active_shop.getCostAdjust());
	rectangle dummy_rect = {0,0,0,0};
	
	switch(item.type) {
		case eShopItemType::EMPTY: break; // Invalid
		case eShopItemType::TREASURE: break; // Also invalid
		case eShopItemType::CLASS: break; // Also invalid
		case eShopItemType::OPT_ITEM: break; // Also invalid
		case eShopItemType::ITEM:
			switch(univ.current_pc().ok_to_buy(cost,base_item)) {
				case eBuyStatus::OK:
					play_sound(-38);
					take_gold(cost,true);
					univ.current_pc().give_item(base_item,true);
					// Magic shops have limited stock
					active_shop.takeOne(i);
					break;
				case eBuyStatus::NO_SPACE: ASB("Can't carry any more items."); break;
				case eBuyStatus::NEED_GOLD: ASB("Not enough cash."); break;
				case eBuyStatus::TOO_HEAVY: ASB("Item is too heavy."); break;
				case eBuyStatus::HAVE_LOTS:
					if(base_item.variety == eItemType::SPECIAL) ASB("You already own this.");
					else if(base_item.variety == eItemType::QUEST) ASB("You already completed this.");
					else ASB("You own too many of this.");
					break;
				// This should not happen:
				case eBuyStatus::DEAD:
					break;
			}
			break;
		case eShopItemType::ALCHEMY:
			if(univ.party.alchemy[base_item.item_level])
				ASB("You already know that recipe.");
			else if(!take_gold(cost,false))
				ASB("Not enough gold.");
			else {
				play_sound(8);
				ASB("You buy an alchemical recipe.");
				univ.party.alchemy[base_item.item_level] = true;
			}
			break;
		case eShopItemType::HEAL_WOUNDS: case eShopItemType::REMOVE_CURSE: case eShopItemType::CURE_DUMBFOUNDING:
		case eShopItemType::CURE_POISON: case eShopItemType::CURE_DISEASE: case eShopItemType::CURE_PARALYSIS:
		case eShopItemType::DESTONE: case eShopItemType::RAISE_DEAD: case eShopItemType::RESURRECT:
		case eShopItemType::CURE_ACID:
			if(!take_gold(cost,false))
				ASB("Not enough gold.");
			else {
				ASB("You pay the healer.");
				play_sound(68);
				switch(item.type) {
					case eShopItemType::HEAL_WOUNDS:
						univ.current_pc().cur_health = univ.current_pc().max_health;
						break;
					case eShopItemType::CURE_POISON:
						univ.current_pc().status[eStatus::POISON] = 0;
						break;
					case eShopItemType::CURE_DISEASE:
						univ.current_pc().status[eStatus::DISEASE] = 0;
						break;
					case eShopItemType::CURE_ACID:
						univ.current_pc().status[eStatus::ACID] = 0;
						break;
					case eShopItemType::CURE_PARALYSIS:
						univ.current_pc().status[eStatus::PARALYZED] = 0;
						break;
					case eShopItemType::REMOVE_CURSE:
						for(int i = 0; i < cPlayer::INVENTORY_SIZE; i++)
							if((univ.current_pc().equip[i]) &&
								(univ.current_pc().items[i].cursed))
								univ.current_pc().items[i].cursed = univ.current_pc().items[i].unsellable = false;
						break;
					case eShopItemType::DESTONE: case eShopItemType::RAISE_DEAD: case eShopItemType::RESURRECT:
						univ.current_pc().main_status = eMainStatus::ALIVE;
						break;
					case eShopItemType::CURE_DUMBFOUNDING:
						if(univ.current_pc().status[eStatus::DUMB] > 0)
							univ.current_pc().status[eStatus::DUMB] = 0;
						break;
					default: break; // Silence compiler warning
				}
			}
			break;
		case eShopItemType::MAGE_SPELL:
			if((base_item.item_level < 0) || (base_item.item_level > 61)) {
				beep();
				showError("The scenario tried to sell you an invalid mage spell!");
				break;
			}
			if(univ.current_pc().mage_spells[base_item.item_level])
				ASB("You already have this spell.");
			else if(!take_gold(cost,false))
				ASB("Not enough gold.");
			else {
				play_sound(25);
				ASB("You buy a spell.");
				univ.current_pc().mage_spells[base_item.item_level] = true;
				give_help(41,0);
			}
			break;
		case eShopItemType::PRIEST_SPELL:
			if((base_item.item_level < 0) || (base_item.item_level > 61)) {
				beep();
				showError("The scenario tried to sell you an invalid priest spell!");
				break;
			}
			if(univ.current_pc().priest_spells[base_item.item_level])
				ASB("You already have this spell.");
			else if(!take_gold(cost,false))
				ASB("Not enough gold.");
			else {
				play_sound(24);
				ASB("You buy a spell.");
				univ.current_pc().priest_spells[base_item.item_level] = true;
				give_help(41,0);
			}
			break;
		case eShopItemType::CALL_SPECIAL:
			if(univ.party.gold < cost)
				ASB("Not enough gold.");
			else {
				short s1;
				run_special(eSpecCtx::SHOPPING, eSpecCtxType::SCEN, base_item.item_level, {0,0}, &s1);
				if(s1 <= 0) {
					take_gold(cost,false);
					active_shop.takeOne(i);
				}
			}
			break;
		case eShopItemType::SKILL:
			if(base_item.item_level < 0 || base_item.item_level > 18) {
				beep();
				showError("The scenario tried to sell you an invalid skill!");
				break;
			}
			eSkill skill = eSkill(base_item.item_level);
			if(univ.current_pc().skills[skill] >= skill_max[skill])
				ASB("You're already an expert in this skill.");
			else if(!take_gold(cost, false))
				ASB("Not enough gold.");
			else {
				play_sound(7);
				ASB("You learn a little...");
				active_shop.takeOne(i);
				univ.current_pc().skills[skill]++;
			}
			break;
	}
	
	if(overall_mode != MODE_SHOPPING) {
		beep();
		ASB("Shop error 1. Report This!");
	}
	// Maybe that was the last of that item, so re-init the shop array just in case.
	set_up_shop_array();
	draw_shop_graphics(false,false,dummy_rect);
	print_buf();
	put_pc_screen();
	put_item_screen(stat_window);

	// This looks to be redundant, but I'm just preserving the previous behavior of the code.
	set_up_shop_array();
	init_shopping_rects(shop_sbar->getMaximum() > 0);
	draw_shop_graphics(false,false,{});

	// When buying from a healer, we want to select the next PC who needs healing
	if(shop_array.empty()){
		start_shop_mode_other_pc(true, true);
	}
}

void handle_info_request(int what_picked) {
	if(recording){
		record_action("handle_info_request", boost::lexical_cast<std::string>(what_picked));
	}
	cShopItem item = active_shop.getItem(what_picked);
	cItem base_item = item.item;
	
	switch(item.type) {
		case eShopItemType::EMPTY: break;
		case eShopItemType::TREASURE: break;
		case eShopItemType::CLASS: break;
		case eShopItemType::OPT_ITEM: break;
		case eShopItemType::ITEM:
			display_pc_item(6,0, base_item,0);
			break;
		case eShopItemType::ALCHEMY:
			// TODO: Create a dedicated dialog for alchemy info
			display_alchemy();
			break;
		case eShopItemType::MAGE_SPELL:
			display_spells(eSkill::MAGE_SPELLS,base_item.item_level,nullptr);
			break;
		case eShopItemType::PRIEST_SPELL:
			display_spells(eSkill::PRIEST_SPELLS,base_item.item_level,nullptr);
			break;
		case eShopItemType::SKILL:
			display_skills(eSkill(base_item.item_level), nullptr);
			break;
		case eShopItemType::HEAL_WOUNDS:
			cStrDlog("Select this option to restore the current PC to full health.", "", "Heal Wounds", 15, PIC_DLOG).show();
			break;
		case eShopItemType::REMOVE_CURSE:
			cStrDlog("Select this option to remove any curses on any items the PC is wearing.", "", "Remove Curse", 15, PIC_DLOG).show();
			break;
		case eShopItemType::CURE_DUMBFOUNDING:
			cStrDlog("Select this option to restore the PC's mind from dumbfounding.", "", "Cure Dumbfounding", 15, PIC_DLOG).show();
			break;
		case eShopItemType::CURE_POISON:
			cStrDlog("Select this option purge all poison from the current PC.", "", "Cure Poison", 15, PIC_DLOG).show();
			break;
		case eShopItemType::CURE_DISEASE:
			cStrDlog("Select this option purge all disease from the current PC.", "", "Cure Disease", 15, PIC_DLOG).show();
			break;
		case eShopItemType::CURE_ACID:
			cStrDlog("Select this option purge all acid from the current PC.", "", "Cure Acid", 15, PIC_DLOG).show();
			break;
		case eShopItemType::CURE_PARALYSIS:
			cStrDlog("Select this option to cure the current PC's paralysis.", "", "Cure Paralysis", 15, PIC_DLOG).show();
			break;
		case eShopItemType::DESTONE:
			cStrDlog("Select this option to restore a PC that has been turned to stone.", "", "Destone", 15, PIC_DLOG).show();
			break;
		case eShopItemType::RAISE_DEAD:
			cStrDlog("Select this option to resurrect a PC.", "", "Raise Dead", 15, PIC_DLOG).show();
			break;
		case eShopItemType::RESURRECT:
			cStrDlog("Select this option to resurrect a PC that has been turned to dust.", "", "Resurrect", 15, PIC_DLOG).show();
			break;
		case eShopItemType::CALL_SPECIAL:
			cStrDlog(base_item.desc, "", base_item.full_name, base_item.graphic_num, PIC_ITEM).show();
			break;
	}
}

void set_up_shop_array() {
	shop_array.clear();
	for(int j = 0; j < active_shop.size(); j++) {
		cShopItem entry = active_shop.getItem(j);
		switch(entry.type) {
			case eShopItemType::ITEM:
			case eShopItemType::MAGE_SPELL:
			case eShopItemType::PRIEST_SPELL:
			case eShopItemType::ALCHEMY:
			case eShopItemType::SKILL:
				shop_array.push_back(j);
				break;
			case eShopItemType::HEAL_WOUNDS:
				if(univ.current_pc().cur_health < univ.current_pc().max_health)
					shop_array.push_back(j);
				break;
			case eShopItemType::CURE_POISON:
				if(univ.current_pc().status[eStatus::POISON] > 0)
					shop_array.push_back(j);
				break;
			case eShopItemType::CURE_DISEASE:
				if(univ.current_pc().status[eStatus::DISEASE] > 0)
					shop_array.push_back(j);
				break;
			case eShopItemType::CURE_ACID:
				if(univ.current_pc().status[eStatus::ACID] > 0)
					shop_array.push_back(j);
				break;
			case eShopItemType::CURE_PARALYSIS:
				if(univ.current_pc().status[eStatus::PARALYZED] > 0)
					shop_array.push_back(j);
				break;
			case eShopItemType::CURE_DUMBFOUNDING:
				if(univ.current_pc().status[eStatus::DUMB] > 0)
					shop_array.push_back(j);
				break;
			case eShopItemType::DESTONE:
				if(univ.current_pc().main_status == eMainStatus::STONE)
					shop_array.push_back(j);
				break;
			case eShopItemType::RAISE_DEAD:
				if(univ.current_pc().main_status == eMainStatus::DEAD)
					shop_array.push_back(j);
				break;
			case eShopItemType::RESURRECT:
				if(univ.current_pc().main_status == eMainStatus::DUST)
					shop_array.push_back(j);
				break;
			case eShopItemType::REMOVE_CURSE:
				for(int i = 0; i < cPlayer::INVENTORY_SIZE; i++) {
					if((univ.current_pc().equip[i]) && (univ.current_pc().items[i].cursed)) {
						shop_array.push_back(j);
						break;
					}
				}
				break;
			case eShopItemType::CALL_SPECIAL:
				if(PSD[entry.item.abil_strength][entry.item.abil_data.value])
					shop_array.push_back(j);
				break;
			case eShopItemType::OPT_ITEM:
				entry.quantity %= 1000;
				BOOST_FALLTHROUGH;
			case eShopItemType::TREASURE:
			case eShopItemType::CLASS:
				entry.type = eShopItemType::ITEM;
				entry.item = univ.party.magic_store_items[active_shop_num][j];
				if(entry.item.variety == eItemType::NO_ITEM)
					entry.type = eShopItemType::EMPTY;
				else shop_array.push_back(j);
				entry.quantity = 1;
				active_shop.replaceItem(j, entry);
				break;
			case eShopItemType::EMPTY:
				break;
		}
	}
	if(shop_array.size() < 8) {
		shop_sbar->setMaximum(0);
	} else {
		shop_sbar->setMaximum(shop_array.size() - 8);
	}
}

std::vector<std::string> preset_words = {
	"Look", "Name", "Job",
	"Buy", "Sell", "Record",
	"Done", "Go Back",
	"Ask About...",
};

static void reset_talk_words() {
	// first initialise talk_words here
	talk_words.clear();
	static const std::vector<location> preset_word_locs = {
		{4, 366}, {70, 366}, {136, 366},
		{4, 389}, {70, 389}, {121, 389},
		{210, 389}, {190, 366},
		{4, 343}
	};
	TextStyle style;
	style.font = FONT_DUNGEON;
	style.pointSize = TALK_WORD_SIZE;
	
	// Place buttons at bottom.
	for(short i = 0; i < preset_words.size(); i++) {
		if(talk_end_forced && i != 6 && i != 5) continue;
		if(!can_save_talk && i == 5) continue;
		if(has_feature_flag("talk-go-back", "StackV1") && dialog_history.size() < 2 && i == 7) continue;

		std::string word = preset_words[i];
		location tl = preset_word_locs[i];
		location br = tl;
		short h;
		br.x += string_length(word, style, &h);
		br.y += h;
		rectangle rect {tl,br};
		word_rect_t preset_word(word, rect);
		preset_word.on = PRESET_WORD_ON;
		preset_word.off = PRESET_WORD_OFF;
		switch(i) {
			case 0: preset_word.node = TALK_LOOK; break;
			case 1: preset_word.node = TALK_NAME; break;
			case 2: preset_word.node = TALK_JOB; break;
			case 3: preset_word.node = TALK_BUY; break;
			case 4: preset_word.node = TALK_SELL; break;
			case 5: preset_word.node = TALK_RECORD; break;
			case 6: preset_word.node = TALK_DONE; break;
			case 7: preset_word.node = TALK_BACK; break;
			case 8: preset_word.node = TALK_ASK; break;
		}
		talk_words.push_back(preset_word);
	}
}

void start_talk_mode(short m_num,short personality,mon_num_t monst_type,short store_face_pic) {
	dialog_history.clear();
	rectangle area_rect;

	store_personality = personality;

	store_monst_type = monst_type;
	store_m_num = m_num;
	store_talk_face_pic = store_face_pic; ////
	area_rect = talk_area_rect;
	talk_gworld().create(area_rect.width(), area_rect.height());
	help_btn->show();

	// This would be the place to show the text box, if I add it.

	// Set the current town for talk strings
	univ.town.prep_talk(personality / 10);

	// Dredge up critter's name
	title_string = std::string(univ.town.cur_talk().people[personality % 10].title) + ":";

	store_pre_talk_mode = overall_mode;
	overall_mode = MODE_TALKING;
	talk_end_forced = false;

	can_save_talk = true;
	reset_talk_words();
	
	stat_screen_mode = MODE_SHOP;
	current_talk_node = TALK_LOOK;
	
	// Bring up and place first strings.
	save_talk_str1 = univ.town.cur_talk().people[personality % 10].look;
	save_talk_str2 = "";
	
	place_talk_str(save_talk_str1, "", 0, dummy_rect);
	dialog_history.push_back({true, false, -1, {save_talk_str1, save_talk_str2}});
	
	put_item_screen(stat_window);
	give_help(5,6);
	
}

void end_talk_mode() {
	// This would be where to hide the text box, if I add it.
	overall_mode = store_pre_talk_mode;
	if(overall_mode == MODE_TALK_TOWN)
		overall_mode = MODE_TOWN;
	if(overall_mode == MODE_TOWN) {
		center = univ.party.town_loc;
		update_explored(center);
	}
	help_btn->hide();
	stat_screen_mode = MODE_INVEN;
	put_item_screen(stat_window);
	put_pc_screen();
	// TODO: I suspect REFRESH_NONE will suffice here
	redraw_screen(REFRESH_TERRAIN | REFRESH_BAR);
	init_inven_rects();
}

static void fill_job_bank(cDialog& me, job_bank_t& bank, std::string) {
	// TODO: Maybe customize the icon?
	// TODO: Allow custom title?
	me["day"].setTextToNum(univ.party.calc_day());
	for(int i = 0; i < 4; i++) {
		std::string id = std::to_string(i + 1);
		if(bank.jobs[i] >= 0 && bank.jobs[i] < univ.scenario.quests.size()) {
			cQuest& quest = univ.scenario.quests[bank.jobs[i]];
			std::string description = quest.descr;
			if(quest.deadline > 0) {
				if(quest.deadline_is_relative)
					description += " Must be completed in " + std::to_string(quest.deadline) + " days.";
				else description += " Must be completed by day " + std::to_string(quest.deadline) + ".";
			}
			description += " Pay is " + std::to_string(quest.gold) + " gold.";
			me["take" + id].show();
			me["job" + id].setText(description);
		} else {
			me["take" + id].hide();
			me["job" + id].setText("");
		}
	}
}

static void show_job_bank(int which_bank, std::string title) {
	cDialog job_dlg(*ResMgr::dialogs.get("job-bank"));
	job_dlg.attachClickHandlers([&](cDialog& me, std::string hit, eKeyMod) -> bool {
		int which = hit[4] - '1';
		me["prompt"].setText("Job accepted.");
		job_bank_t& bank = univ.party.job_banks[which_bank];
		univ.party.active_quests[bank.jobs[which]] = cJob(univ.party.calc_day(), store_personality);
		// Now, if there are spare jobs available, fill in. Otherwise, clear space.
		if(bank.jobs[4] >= 0)
			std::swap(bank.jobs[which], bank.jobs[4]);
		else if(bank.jobs[5] >= 0)
			std::swap(bank.jobs[which], bank.jobs[5]);
		fill_job_bank(me, bank, title);
		return true;
	}, {"take1", "take2", "take3", "take4"});
	job_dlg["done"].attachClickHandler(std::bind(&cDialog::toast, &job_dlg, false));
	
	if(which_bank >= univ.party.job_banks.size())
		univ.party.job_banks.resize(which_bank + 1);
	if(!univ.party.job_banks[which_bank].inited)
		univ.generate_job_bank(which_bank, univ.party.job_banks[which_bank]);
	fill_job_bank(job_dlg, univ.party.job_banks[which_bank], title);
	
	int anger = univ.party.job_banks[which_bank].anger;
	if(anger >= 0 && anger < 10) {
		job_dlg["prompt"].setText("Dispatcher is neutral towards you.");
	} else if(anger >= 10 && anger < 20) {
		job_dlg["prompt"].setText("Dispatcher is a little annoyed at you.");
	} else if(anger >= 20 && anger < 35) {
		job_dlg["prompt"].setText("Dispatcher is annoyed at you.");
	} else job_dlg["prompt"].setText("Dispatcher is rather angry at you.");
	
	job_dlg.run();
}

void handle_talk_node(int which_talk_entry, bool is_redo) {
	if(which_talk_entry == TALK_DUNNO)
		return;

	short get_pc,s1 = -1,s2 = -1;
	char asked[4];
	
	short a,b,c,d;
	eTalkNode ttype;
	
	switch(which_talk_entry) {
		case TALK_DUNNO:
		SPECIAL_DUNNO:
			save_talk_str1 = univ.town.cur_talk().people[store_personality % 10].dunno;
			save_talk_str2 = "";
			if(save_talk_str1.length() < 2) save_talk_str1 = "You get no response.";
			update_last_talk(TALK_DUNNO);
			goto FINISH_TALK_NODE;
		case TALK_BUSINESS: // This one only reachable via "go back".
			can_save_talk = false;
			save_talk_str1 = "You conclude your business.";
			save_talk_str2 = "";
			update_last_talk(TALK_BUSINESS);
			goto FINISH_TALK_NODE;
		case TALK_LOOK:
		SPECIAL_LOOK:
			save_talk_str1 = univ.town.cur_talk().people[store_personality % 10].look;
			save_talk_str2 = "";
			update_last_talk(TALK_LOOK);
			goto FINISH_TALK_NODE;
		case TALK_NAME:
		SPECIAL_NAME:
			save_talk_str1 = univ.town.cur_talk().people[store_personality % 10].name;
			save_talk_str2 = "";
			update_last_talk(TALK_NAME);
			goto FINISH_TALK_NODE;
		case TALK_JOB:
		SPECIAL_JOB:
			save_talk_str1 = univ.town.cur_talk().people[store_personality % 10].job;
			save_talk_str2 = "";
			update_last_talk(TALK_JOB);
			goto FINISH_TALK_NODE;
		case TALK_BUY:
		SPECIAL_BUY:
			which_talk_entry = scan_for_response("purc");
			if(which_talk_entry < 0) which_talk_entry = scan_for_response("sale");
			if(which_talk_entry < 0) which_talk_entry = scan_for_response("heal");
			if(which_talk_entry < 0) which_talk_entry = scan_for_response("iden");
			if(which_talk_entry < 0) which_talk_entry = scan_for_response("trai");
			if(which_talk_entry < 0) which_talk_entry = scan_for_response("ench");
			if(which_talk_entry == -1) goto SPECIAL_DUNNO;
			break;
		case TALK_SELL:
		SPECIAL_SELL:
			which_talk_entry = scan_for_response("sell");
			if(which_talk_entry == -1) goto SPECIAL_DUNNO;
			break;
		case TALK_RECORD:
			if(univ.party.save_talk(univ.town->talking.people[store_personality%10].title, univ.town->name, save_talk_str1, save_talk_str2)) {
				give_help(57,0);
				play_sound(0);
				ASB("Added to conversation notes.");
			} else ASB("This is already saved.");
			print_buf();
			return;
		case TALK_DONE:
		SPECIAL_DONE:
			end_talk_mode();
			return;
		case TALK_BACK:
			// In legacy behavior, this should only happen if there's nothing to go back to. In that case, do nothing.
			if(has_feature_flag("talk-go-back", "StackV1")){
				if(!dialog_history.empty()){
					dialog_history_t last_state;
					do{
						last_state = dialog_history.back();
						dialog_history.pop_back();
						if(last_state.special_node){
							handle_talk_node(last_state.node, true);
							return;
						}
					// If they ended up in the same node, don't treat it as a step back until we get to different text.
					}while(!dialog_history.empty() && save_talk_str1 == last_state.text.first && save_talk_str2 == last_state.text.second);
					can_save_talk = last_state.can_record;
					save_talk_str1 = last_state.text.first;
					save_talk_str2 = last_state.text.second;
					goto FINISH_TALK_NODE;
				}
			}
			return;
		case TALK_ASK: // ask about
			save_talk_str1 = get_text_response("Ask about what?", 8);
			strncpy(asked, save_talk_str1.c_str(), 4);
			if(strnicmp(asked, "name", 4) == 0) goto SPECIAL_NAME;
			if(strnicmp(asked, "look", 4) == 0) goto SPECIAL_LOOK;
			if(strnicmp(asked, "job", 3) == 0)  goto SPECIAL_JOB;
			if(strnicmp(asked, "work", 4) == 0) goto SPECIAL_JOB;
			if(strnicmp(asked, "bye", 3) == 0)  goto SPECIAL_DONE;
			if(strnicmp(asked, "buy", 3) == 0)  goto SPECIAL_BUY;
			if(strnicmp(asked, "sell", 4) == 0) goto SPECIAL_SELL;
			which_talk_entry = scan_for_response(asked);
			if(which_talk_entry == -1) goto SPECIAL_DUNNO;
			break;
	}
	update_last_talk(which_talk_entry);
	
	ttype = univ.town.cur_talk().talk_nodes[which_talk_entry].type;
	a = univ.town.cur_talk().talk_nodes[which_talk_entry].extras[0];
	b = univ.town.cur_talk().talk_nodes[which_talk_entry].extras[1];
	c = univ.town.cur_talk().talk_nodes[which_talk_entry].extras[2];
	d = univ.town.cur_talk().talk_nodes[which_talk_entry].extras[3];
	
	save_talk_str1 = univ.town.cur_talk().talk_nodes[which_talk_entry].str1;
	save_talk_str2 = univ.town.cur_talk().talk_nodes[which_talk_entry].str2;
	
	can_save_talk = true;
	
	switch(ttype) {
		case eTalkNode::REGULAR:
			break;
		case eTalkNode::DEP_ON_SDF:
			if(PSD[a][b] > c) {
				save_talk_str1 = save_talk_str2;
			}
			save_talk_str2 = "";
			break;
		case eTalkNode::SET_SDF:
			PSD[a][b] = c;
			break;
		case eTalkNode::INN:
			if(univ.party.gold < a) {
				save_talk_str1 = save_talk_str2;
			}
			else {
				talk_end_forced = true;
				univ.party.gold -= a;
				put_pc_screen();
				do_rest(700, 30 * b, 25 * b);
				univ.party.town_loc.x = c;
				univ.party.town_loc.y = d;
				center = univ.party.town_loc;
			}
			save_talk_str2 = "";
			break;
		case eTalkNode::DEP_ON_TIME:
			if(day_reached((unsigned char) a,0)) {
				save_talk_str1 = save_talk_str2;
			}
			save_talk_str2 = "";
			break;
		case eTalkNode::DEP_ON_TIME_AND_EVENT:
			if(day_reached((unsigned char) a,(unsigned char) b)) {
				save_talk_str1 = save_talk_str2;
			}
			save_talk_str2 = "";
			break;
		case eTalkNode::DEP_ON_TOWN:
			if(univ.party.town_num != a) {
				save_talk_str1 = save_talk_str2;
			}
			save_talk_str2 = "";
			break;
		case eTalkNode::TRAINING:
			save_talk_str1 = "You conclude your training.";
			save_talk_str2 = "";
			can_save_talk = false;
			if((get_pc = select_pc(eSelectPC::ONLY_CAN_TRAIN,"Train who?")) < 6) {
				spend_xp(get_pc,1, nullptr);
			}
			goto RECORD_WHICH_NODE;
		case eTalkNode::SHOP:
			if(!start_shop_mode(b,a,save_talk_str1,true)){
				if(!start_shop_mode_other_pc()){
					univ.cur_pc = store_cur_pc;

					// Second string of shop talk node: Custom message for when shop is empty
					if(!save_talk_str2.empty()){
						save_talk_str1 = save_talk_str2;
						save_talk_str2 = "";
					}else{
						save_talk_str1 = "There is nothing available to buy.";
					}
				}
			}else{
				can_save_talk = false;
			}
			goto RECORD_WHICH_NODE;
		case eTalkNode::JOB_BANK:
			if(a < univ.party.job_banks.size() && univ.party.job_banks[a].anger >= 50) {
				save_talk_str1 = save_talk_str2;
				save_talk_str2 = "";
				break;
			} else {
				show_job_bank(a, save_talk_str1.c_str());
				goto RECORD_WHICH_NODE;
			}
		case eTalkNode::SELL_WEAPONS:
			can_save_talk = false;
			stat_screen_mode = MODE_SELL_WEAP;
			put_item_screen(stat_window);
			give_help(42,43);
			goto RECORD_WHICH_NODE;
		case eTalkNode::SELL_ARMOR:
			can_save_talk = false;
			stat_screen_mode = MODE_SELL_ARMOR;
			put_item_screen(stat_window);
			give_help(42,43);
			goto RECORD_WHICH_NODE;
		case eTalkNode::SELL_ITEMS:
			can_save_talk = false;
			stat_screen_mode = MODE_SELL_ANY;
			put_item_screen(stat_window);
			give_help(42,43);
			goto RECORD_WHICH_NODE;
		case eTalkNode::IDENTIFY:
			stat_screen_mode = MODE_IDENTIFY,
			give_help(44, 0);
			if(false) // Skip first line of fallthrough
		case eTalkNode::ENCHANT:
			stat_screen_mode = MODE_ENCHANT,
			give_help(45, 0);
			if(false) // Skip first line of fallthrough
		case eTalkNode::RECHARGE:
			stat_screen_mode = MODE_RECHARGE,
			give_help(66, 0);
			can_save_talk = false;
			shop_identify_cost = a;
			shop_recharge_limit = b;
			shop_recharge_amount = c;
			put_item_screen(stat_window);
			goto RECORD_WHICH_NODE;
		case eTalkNode::BUY_INFO:
			if(univ.party.gold < a) {
				save_talk_str1 = save_talk_str2;
			}
			else {
				univ.party.gold -= a;
				put_pc_screen();
				
			}
			save_talk_str2 = "";
			break;
		case eTalkNode::BUY_SDF:
			if((univ.party.sd_legit(b,c)) && (PSD[b][c] == d)) {
				save_talk_str1 = "You've already learned that.";
				can_save_talk = false;
			}
			else if(univ.party.gold < a) {
				save_talk_str1 = save_talk_str2;
			}
			else {
				univ.party.gold -= a;
				put_pc_screen();
				if(univ.party.sd_legit(b,c))
					PSD[b][c] = d;
				else showError("Invalid Stuff Done flag called in conversation.");
			}
			save_talk_str2 = "";
			break;
		case eTalkNode::BUY_SHIP:
			if(univ.party.gold < a) {
				save_talk_str1 = save_talk_str2;
				save_talk_str2 = "";
				break;
			} else {
				using namespace std::placeholders;
				auto& boats = univ.party.boats;
				b = minmax(0, boats.size() - 1, b);
				c = minmax(0, boats.size() - b, c);
				auto iter = std::find_if(boats.begin() + b, boats.begin() + b + c, std::bind(&cVehicle::property, _1));
				if(iter != boats.end()) {
					univ.party.gold -= a;
					put_pc_screen();
					iter->property = false;
					save_talk_str2 = "";
				} else {
					save_talk_str1 = "There are no boats left.";
					save_talk_str2 = "";
					can_save_talk = false;
				}
			}
			break;
		case eTalkNode::BUY_HORSE:
			if(univ.party.gold < a) {
				save_talk_str1 = save_talk_str2;
				save_talk_str2 = "";
				break;
			} else {
				using namespace std::placeholders;
				auto& horses = univ.party.horses;
				b = minmax(0, horses.size() - 1, b);
				c = minmax(0, horses.size() - b, c);
				auto iter = std::find_if(horses.begin() + b, horses.begin() + b + c, std::bind(&cVehicle::property, _1));
				if(iter != horses.end()) {
					univ.party.gold -= a;
					put_pc_screen();
					iter->property = false;
				} else {
					save_talk_str1 = "There are no horses left.";
					can_save_talk = false;
				}
				save_talk_str2 = "";
			}
			break;
		case eTalkNode::BUY_SPEC_ITEM:
			if(univ.party.spec_items.count(a)) {
				save_talk_str1 = "You already have it.";
				can_save_talk = false;
			}
			else if(univ.party.gold < b) {
				save_talk_str1 = save_talk_str2;
			}
			else {
				univ.party.gold -= b;
				put_pc_screen();
				univ.party.spec_items.insert(a);
			}
			save_talk_str2 = "";
			break;
		case eTalkNode::RECEIVE_QUEST:
			if(a < 0 || a >= univ.scenario.quests.size()) {
				showError("Tried to give a nonexistent quest!");
				return;
			}
			switch(univ.party.active_quests[a].status) {
				case eQuestStatus::AVAILABLE:
					univ.party.active_quests[a] = cJob(univ.party.calc_day());
					break;
				case eQuestStatus::STARTED:
					break;
				case eQuestStatus::COMPLETED:
					save_talk_str1 = save_talk_str2;
					break;
				case eQuestStatus::FAILED:
					// TODO: How to handle this?
					return;
			}
			save_talk_str2 = "";
			break;
		case eTalkNode::BUY_TOWN_LOC:
			if(univ.scenario.towns[b]->can_find) {
				save_talk_str1 = "You've already learned that.";
			}
			else if(univ.party.gold < a) {
				save_talk_str1 = save_talk_str2;
			}
			else {
				univ.party.gold -= a;
				put_pc_screen();
				univ.scenario.towns[b]->can_find = true;
			}
			save_talk_str2 = "";
			break;
		case eTalkNode::END_FORCE:
			talk_end_forced = true;
			break;
		case eTalkNode::END_FIGHT:
			if(store_m_num >= 0 && store_m_num < univ.town.monst.size()) {
				univ.town.monst[store_m_num].attitude = eAttitude::HOSTILE_A;
				univ.town.monst[store_m_num].mobility = 1;
			}
			talk_end_forced = true;
			break;
		case eTalkNode::END_ALARM:
			make_town_hostile();
			talk_end_forced = true;
			break;
		case eTalkNode::END_DIE:
			if(store_m_num >= 0 && store_m_num < univ.town.monst.size()) {
				// TODO: Any reason not to call something like kill_monst?
				univ.town.monst[store_m_num].active = eCreatureStatus::DEAD;
				// Special killing effects
				if(univ.party.sd_legit(univ.town.monst[store_m_num].spec1,univ.town.monst[store_m_num].spec2))
					PSD[univ.town.monst[store_m_num].spec1][univ.town.monst[store_m_num].spec2] = 1;
			}
			talk_end_forced = true;
			break;
			// TODO: Strings resulting from this don't seem to be recordable; whyever not!?
		case eTalkNode::CALL_TOWN_SPEC:
			run_special(eSpecCtx::TALK, eSpecCtxType::TOWN, a, univ.party.town_loc, &s1, &s2);
			// check s1 & s2 to see if we got diff str, and, if so, munch old strs
			if((s1 >= 0) || (s2 >= 0)) {
				save_talk_str1 = s1 >= 0 ? univ.town->spec_strs[s1] : "";
				save_talk_str2 = s2 >= 0 ? univ.town->spec_strs[s2] : "";
			}
			univ.get_strs(save_talk_str1, save_talk_str2, eSpecCtxType::TOWN, s1, s2);
			put_pc_screen();
			put_item_screen(stat_window);
			break;
		case eTalkNode::CALL_SCEN_SPEC:
			run_special(eSpecCtx::TALK, eSpecCtxType::SCEN, a, univ.party.town_loc, &s1, &s2);
			// check s1 & s2 to see if we got diff str, and, if so, munch old strs
			if((s1 >= 0) || (s2 >= 0)) {
				save_talk_str1 = s1 >= 0 ? univ.scenario.spec_strs[s1] : "";
				save_talk_str2 = s2 >= 0 ? univ.scenario.spec_strs[s2] : "";
			}
			univ.get_strs(save_talk_str1, save_talk_str2, eSpecCtxType::SCEN, s1, s2);
			put_pc_screen();
			put_item_screen(stat_window);
			break;
	}

	if(false){
	RECORD_WHICH_NODE:
		if(!is_redo) dialog_history.push_back({false, true, which_talk_entry, {"", ""}});
		place_talk_str(save_talk_str1,save_talk_str2,0,dummy_rect);
		reset_talk_words();
		return;
	}

FINISH_TALK_NODE:
	dialog_history.push_back({can_save_talk, false, -1, {save_talk_str1, save_talk_str2}});
	place_talk_str(save_talk_str1,save_talk_str2,0,dummy_rect);
	reset_talk_words();
	return;
}

bool handle_talk_event(location p, cFramerateLimiter& fps_limiter) {
	if(p.in(talk_help_rect)) {
		if(help_btn->handleClick(p, fps_limiter))
			give_help_and_record(5,6,true);
		return true;
	}

	p.x -= 5;
	p.y -= 5;

	int which_talk_entry = TALK_DUNNO;
	bool clicked_word = false;
	for(word_rect_t& word : talk_words) {
		if(word.node == -1) continue;
		rectangle wordRect(word.rect);
		wordRect.offset(talk_area_rect.topLeft());
		wordRect.offset(-1, -10); // TODO: This corrects for the byzantine offsets that win_draw_string applies for some reason...
		if(!p.in(wordRect)) continue;
		click_talk_rect(word);
		which_talk_entry = word.node;
		clicked_word = true;
		break;
	}
	handle_talk_node(which_talk_entry);
	// Update hitboxes of the inventory in case sell/identify/recharge mode started
	init_inven_rects();
	return clicked_word || p.in(rectangle(talk_gworld()));
}

//town_num; // Will be 0 - 200 for town, 200 - 290 for outdoors
//short sign_type; // terrain type
void do_sign(short town_num, short which_sign, short sign_type) {
	std::string sign_text;
	
	set_cursor(sword_curs);
	
	cChoiceDlog sign("view-sign");
	cPict& pict = dynamic_cast<cPict&>(sign->getControl("ter"));
	
	store_sign_mode = sign_type;
	pict.setPict(univ.scenario.ter_types[sign_type].picture);
	
	// TODO: Why is town_num unused?
	// Seems like it could cause the wrong sign to show sometimes,
	// like when you're in one outdoor section viewing a sign in
	// an adjacent section
	if(town_num >= 200) {
		town_num -= 200;
		sign_text = univ.out->sign_locs[which_sign].text;
	}
	else {
		sign_text = univ.town->sign_locs[which_sign].text;
	}
	(void) town_num;
	sign->getControl("sign").setText(sign_text);
	
	sign.show();
}

void save_prefs(){
	bool success = sync_prefs();
	if(!success){
		showWarning("There was a problem writing to the preferences file. When the game is next run the preferences will revert to their previously set values.","Should you manage to resolve the problem without closing the program, simply open the preferences screen and click \"OK\" to try again.");
	}
}

void autosave_preferences(cDialog* parent);

static bool prefs_autosave_event_filter(cDialog& me, std::string id, eKeyMod mod) {
	if(id == "autosave-toggle"){
		dynamic_cast<cLed&>(me["autosave-toggle"]).defaultClickHandler(me, id, mod);
		if(dynamic_cast<cLed&>(me["autosave-toggle"]).getState() != led_off){
			me["autosave-details"].show();
		}
		else{
			me["autosave-details"].hide();
		}
	}else if(id == "autosave-details"){
		autosave_preferences(&me);
	}
	// Messy: Using the same handler for the autosave buttons in the main preferences,
	// and the buttons in the autosave details window.
	else{
		bool did_cancel = false;
		bool save_ok = false;

		if(id == "okay") {
			save_ok = me.toast(true);
		} else if(id == "cancel") {
			me.toast(false);
			did_cancel = true;
		}

		if(!did_cancel && save_ok) {
			set_pref("Autosave_Max", std::stoi(dynamic_cast<cTextField&>(me["max-files"]).getText()));
			for(cDialogIterator iter = me.begin(); iter != me.end(); ++iter){
				std::string id = iter->first;
				cControl* ctrl = iter->second;
				cLed* led = dynamic_cast<cLed*>(ctrl);
				if(led != nullptr){
					set_pref("Autosave_" + id, led->getState() != led_off);
				}
			}
			save_prefs();
		}
	}
	return true;
}

void autosave_preferences(cDialog* parent) {
	cDialog prefsDlog(*ResMgr::dialogs.get("pref-autosave"), parent);

	int max_autosaves = get_int_pref("Autosave_Max", MAX_AUTOSAVE_DEFAULT);
	cTextField& max_files = dynamic_cast<cTextField&>(prefsDlog["max-files"]);
	max_files.setText(std::to_string(max_autosaves));

	for(cDialogIterator iter = prefsDlog.begin(); iter != prefsDlog.end(); ++iter){
		std::string id = iter->first;
		cControl* ctrl = iter->second;
		cLed* led = dynamic_cast<cLed*>(ctrl);
		if(led != nullptr){
			led->setState(check_autosave_trigger(id) ? led_red : led_off);
		}
	}
	prefsDlog.attachClickHandlers(&prefs_autosave_event_filter, {"okay", "cancel"});
	prefsDlog.run();
}


static bool prefs_event_filter (cDialog& me, std::string id, eKeyMod) {
	bool did_cancel = false;
	
	if(id == "okay") {
		me.toast(true);
	} else if(id == "cancel") {
		me.toast(false);
		did_cancel = true;
	}
	
	if(!did_cancel) {
		std::string cur_display_mode = dynamic_cast<cLedGroup&>(me["display"]).getSelected();
		// TODO: Make an enum for the display mode
		if(cur_display_mode == "mid") set_pref("DisplayMode", 0);
		else if(cur_display_mode == "tl") set_pref("DisplayMode", 1);
		else if(cur_display_mode == "tr") set_pref("DisplayMode", 2);
		else if(cur_display_mode == "bl") set_pref("DisplayMode", 3);
		else if(cur_display_mode == "br") set_pref("DisplayMode", 4);
		else if(cur_display_mode == "win") set_pref("DisplayMode", 5);
		set_pref("PlaySounds", dynamic_cast<cLed&>(me["nosound"]).getState() == led_off);

		set_pref("DirectionalKeyScrolling", dynamic_cast<cLed&>(me["screen-shift"]).getState() != led_off);
		set_pref("FancyFilePicker", dynamic_cast<cLed&>(me["fancypicker"]).getState() != led_off);
		set_pref("Autosave", dynamic_cast<cLed&>(me["autosave-toggle"]).getState() != led_off);
		set_pref("ShowInstantHelp", dynamic_cast<cLed&>(me["nohelp"]).getState() == led_off);
		
		if(overall_mode == MODE_STARTUP && !party_in_memory) {
			set_pref("EasyMode", dynamic_cast<cLed&>(me["easier"]).getState() != led_off);
			set_pref("LessWanderingMonsters", dynamic_cast<cLed&>(me["lesswm"]).getState() != led_off);
		} else {
			univ.party.easy_mode = dynamic_cast<cLed&>(me["easier"]).getState() != led_off;
			univ.party.less_wm = dynamic_cast<cLed&>(me["lesswm"]).getState() != led_off;
		}
		set_pref("ShowStartupSplash", dynamic_cast<cLed&>(me["skipsplash"]).getState() == led_off);
		set_pref("TargetLock", dynamic_cast<cLed&>(me["target-lock"]).getState() != led_off);

		std::string speed = dynamic_cast<cLedGroup&>(me["speed"]).getSelected();
		/* TODO: Should I add these additional preferences from Windows?
		party.stuff_done[SDF_NO_TARGET_LINE] = cd_get_led(1099,50);
		party.stuff_done[SDF_LESS_SOUND] = cd_get_led(1099,52);
		party.stuff_done[SDF_FASTER_BOOM_SPACES] = cd_get_led(1099,56);
		party.stuff_done[SDF_ASK_ABOUT_TEXT_BOX] = cd_get_led(1099,60);
		*/
		if(speed == "fast")
			set_pref("GameSpeed", 0);
		else if(speed == "med")
			set_pref("GameSpeed", 1);
		else if(speed == "slow")
			set_pref("GameSpeed", 2);
		else if(speed == "snail")
			set_pref("GameSpeed", 3);
		std::string scale = dynamic_cast<cLedGroup&>(me["scaleui"]).getSelected();
		if(scale == "1")
			set_pref("UIScale", 1.0);
		else if(scale == "1_5")
			set_pref("UIScale", 1.5);
		else if(scale == "2")
			set_pref("UIScale", 2.0);
		else if(scale == "3")
			set_pref("UIScale", 3.0);
		else if(scale == "4")
			set_pref("UIScale", 4.0);
		std::string scale_map = dynamic_cast<cLedGroup&>(me["scalemap"]).getSelected();
		if(scale_map == "1")
			set_pref("UIScaleMap", 1.0);
		else if(scale_map == "1_5")
			set_pref("UIScaleMap", 1.5);
		else if(scale_map == "2")
			set_pref("UIScaleMap", 2.0);
		else if(scale_map == "3")
			set_pref("UIScaleMap", 3.0);
		else if(scale_map == "4")
			set_pref("UIScaleMap", 4.0);
	}
	save_prefs();
	return true;
}

static bool reset_help(cDialog& me, std::string, eKeyMod) {
	if(cChoiceDlog("confirm-reset-help", {"yes", "no"}, &me).show() == "yes")
		clear_pref("ReceivedHelp");
	return true;
}

void pick_preferences(bool record) {
	if(record && recording){
		record_action("pick_preferences", "");
	}

	set_cursor(sword_curs);
	
	cDialog prefsDlog(*ResMgr::dialogs.get("preferences"));
	prefsDlog.attachClickHandlers(&prefs_autosave_event_filter, {"autosave-toggle", "autosave-details"});
	prefsDlog.attachClickHandlers(&prefs_event_filter, {"okay", "cancel"});
	prefsDlog.attachClickHandlers(&reset_help, {"resethelp"});

	cLedGroup& displayMode = dynamic_cast<cLedGroup&>(prefsDlog["display"]);
	switch(get_int_pref("DisplayMode")) {
		case 0:
			displayMode.setSelected("mid");
			break;
		case 1:
			displayMode.setSelected("tl");
			break;
		case 2:
			displayMode.setSelected("tr");
			break;
		case 3:
			displayMode.setSelected("bl");
			break;
		case 4:
			displayMode.setSelected("br");
			break;
		case 5:
			displayMode.setSelected("win");
			break;
	}
	
	dynamic_cast<cLed&>(prefsDlog["nosound"]).setState(get_bool_pref("PlaySounds", true) ? led_off : led_red);
	dynamic_cast<cLed&>(prefsDlog["fancypicker"]).setState(get_bool_pref("FancyFilePicker", true) ? led_red : led_off);
	bool autosave_on = get_bool_pref("Autosave", true);
	dynamic_cast<cLed&>(prefsDlog["autosave-toggle"]).setState(autosave_on ? led_red : led_off);
	if(!autosave_on)
		prefsDlog["autosave-details"].hide();
	dynamic_cast<cLed&>(prefsDlog["nohelp"]).setState(get_bool_pref("ShowInstantHelp", true) ? led_off : led_red);
	if(overall_mode == MODE_STARTUP && !party_in_memory) {
		dynamic_cast<cLed&>(prefsDlog["easier"]).setState(get_bool_pref("EasyMode") ? led_red : led_off);
		dynamic_cast<cLed&>(prefsDlog["lesswm"]).setState(get_bool_pref("LessWanderingMonsters") ? led_red : led_off);
	} else {
		dynamic_cast<cLed&>(prefsDlog["easier"]).setState(univ.party.easy_mode ? led_red : led_off);
		dynamic_cast<cLed&>(prefsDlog["lesswm"]).setState(univ.party.less_wm ? led_red : led_off);
	}
	dynamic_cast<cLed&>(prefsDlog["skipsplash"]).setState(get_bool_pref("ShowStartupSplash", true) ? led_off : led_red);
	cLedGroup& gameSpeed = dynamic_cast<cLedGroup&>(prefsDlog["speed"]);
	switch(get_int_pref("GameSpeed")) {
		case 0:
			gameSpeed.setSelected("fast");
			break;
		case 1:
			gameSpeed.setSelected("med");
			break;
		case 2:
			gameSpeed.setSelected("slow");
			break;
		case 3:
			gameSpeed.setSelected("snail");
			break;
	}

	dynamic_cast<cLed&>(prefsDlog["target-lock"]).setState(get_bool_pref("TargetLock", true) ? led_red : led_off);	

	cLedGroup& keyshiftOptions = dynamic_cast<cLedGroup&>(prefsDlog["keyshift-options"]);
	if(get_bool_pref("DirectionalKeyScrolling", false)){
		keyshiftOptions.setSelected("screen-shift");
	}else{
		keyshiftOptions.setSelected("target-adjacent");
	}

	cLedGroup& uiScale = dynamic_cast<cLedGroup&>(prefsDlog["scaleui"]);
	double ui_scale = get_ui_scale();
	if (ui_scale>0.95 && ui_scale<1.05) uiScale.setSelected("1");
	else if (ui_scale>1.45 && ui_scale<1.55) uiScale.setSelected("1_5");
	else if (ui_scale>1.95 && ui_scale<2.05) uiScale.setSelected("2");
	else if (ui_scale>2.95 && ui_scale<3.05) uiScale.setSelected("3");
	else if (ui_scale>3.95 && ui_scale<4.05) uiScale.setSelected("4");
	else uiScale.setSelected("other");

	// Disable buttons for scales that won't fit on player's screen:
	double max_main_window_scale = fallback_scale();
	if(max_main_window_scale < 4.0){
		uiScale["4"].hide();
	}
	if(max_main_window_scale < 3.0){
		uiScale["3"].hide();
	}
	if(max_main_window_scale < 2.0){
		uiScale["2"].hide();
	}
	if(max_main_window_scale < 1.5){
		uiScale["1_5"].hide();
	}

	cLedGroup& uiMapScale = dynamic_cast<cLedGroup&>(prefsDlog["scalemap"]);
	double ui_map_scale = get_ui_scale_map();
	if (ui_map_scale>0.95 && ui_map_scale<1.05) uiMapScale.setSelected("1");
	else if (ui_map_scale>1.45 && ui_map_scale<1.55) uiMapScale.setSelected("1_5");
	else if (ui_map_scale>1.95 && ui_map_scale<2.05) uiMapScale.setSelected("2");
	else if (ui_map_scale>2.95 && ui_map_scale<3.05) uiMapScale.setSelected("3");
	else if (ui_map_scale>3.95 && ui_map_scale<4.05) uiMapScale.setSelected("4");
	else uiMapScale.setSelected("other");
	
	// The map is smaller and it doesn't matter if the player makes it too big, so always show all options.

	if(uiScale.getSelected() == "other") {
		auto val = std::to_string(ui_scale);
		while(val.length() > 2 && val[val.length() - 1] == val[val.length() - 2]) {
			val.pop_back();
		}
		while(val.length() > 1 && (val.back() == '.' || val.back() == '0')) {
			val.pop_back();
		}
		uiScale["other"].setText("Custom: " + val);
	} else {
		uiScale["other"].hide();
	}
	if(uiMapScale.getSelected() == "other") {
		auto val = std::to_string(ui_map_scale);
		while(val.length() > 2 && val[val.length() - 1] == val[val.length() - 2]) {
			val.pop_back();
		}
		while(val.length() > 1 && (val.back() == '.' || val.back() == '0')) {
			val.pop_back();
		}
		uiMapScale["other"].setText("Custom: " + val);
	} else {
		uiMapScale["other"].hide();
	}
	
	int store_display_mode = get_int_pref("DisplayMode");
	prefsDlog.runWithHelp(55, 0);
	
	// Suppress the float comparison warning.
 	// We know it's safe here - we're just comparing static values.
	#ifdef __GNUC__
	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wfloat-equal"
	#endif
	if(get_int_pref("DisplayMode") != store_display_mode || get_ui_scale() != ui_scale || get_ui_scale_map() != ui_map_scale)
		changed_display_mode = true;
	#ifdef __GNUC__
	#pragma GCC diagnostic pop
	#endif
}

static void put_party_stats(cDialog& me) {
	for(short i = 0; i < 6; i++) {
		std::string n = boost::lexical_cast<std::string>(i + 1);
		if(univ.party[i].main_status != eMainStatus::ABSENT) {
			me["name" + n].setText(univ.party[i].name);
			me["trait" + n].show();
			me["train" + n].show();
			me["delete" + n].setText("Delete");
			me["pic" + n].show();
			me["pc" + n].show();
			dynamic_cast<cPict&>(me["pc" + n]).setPict(univ.party[i].which_graphic);
		}
		else {
			me["name" + n].setText("Empty.");
			me["trait" + n].hide();
			me["train" + n].hide();
			me["delete" + n].setText("Create");
			me["pic" + n].hide();
			me["pc" + n].hide();
		}
		
	}
	draw_startup(0);
}

static bool edit_party_event_filter(cDialog& me, std::string item_hit, eKeyMod) {
	if(item_hit == "done") {
		me.toast(true);
	} else if(item_hit == "help") {
		give_help(22,23,me,true);
	} else {
		short which_pc = item_hit[item_hit.length()-1] - '1';
		item_hit = item_hit.substr(0, item_hit.length() - 1);
		if(item_hit == "name") {
			if(univ.party[which_pc].main_status == eMainStatus::ABSENT) {
				give_help(56,0,me);
				create_pc(which_pc,&me);
				put_party_stats(me);
			}
			else pick_pc_name(which_pc,&me);
			put_party_stats(me);
		} else if(item_hit == "trait") {
			pick_race_abil(&univ.party[which_pc],0,&me);
			put_party_stats(me);
		} else if(item_hit == "train") {
			spend_xp(which_pc,0,&me);
			put_party_stats(me);
		} else if(item_hit == "pic") {
			if(univ.party[which_pc].main_status == eMainStatus::ABSENT)
				return true;
			pick_pc_graphic(which_pc,1,&me);
			put_party_stats(me);
		} else if(item_hit == "delete") { // Note: This button is also used for "create new PC".
			if(univ.party[which_pc].main_status != eMainStatus::ABSENT) {
				if(cChoiceDlog("delete-pc-confirm",{"yes","no"},&me).show() == "yes")
					univ.party[which_pc].main_status = eMainStatus::ABSENT;
				put_party_stats(me);
			}
			else {
				give_help(56,0,me);
				create_pc(which_pc,&me);
				put_party_stats(me);
			}
		}
	}
	return true;
}

void edit_party() {
	set_cursor(sword_curs);
	
	cDialog pcDialog(*ResMgr::dialogs.get("edit-party"));
	std::vector<std::string> buttons = {"done", "help"};
	for(int i = 1; i <= 6; i++) {
		std::string n = boost::lexical_cast<std::string>(i);
		buttons.push_back("name" + n);
		buttons.push_back("delete" + n);
		buttons.push_back("trait" + n);
		buttons.push_back("train" + n);
		buttons.push_back("pic" + n);
	}
	
	pcDialog.attachClickHandlers(edit_party_event_filter, buttons);
	
	put_party_stats(pcDialog);
	
	pcDialog.runWithHelp(22, 23);
	
	if(univ.current_pc().main_status != eMainStatus::ALIVE)
		univ.cur_pc = first_active_pc();
	
	
}

static bool tip_of_day_event_filter(cDialog& me, std::string item_hit, short& page) {
	std::string place_str;
	
	if(item_hit == "done") {
		me.toast(true);
	} else if(item_hit == "next") {
		page++;
		if(page == ResMgr::strings.get("tips")->size() - 50)
			page = 0;
		place_str = get_str("tips",50 + page);
		me["tip"].setText(place_str);
	}
	return true;
}

void tip_of_day() {
	if(recording){
		record_action("tip_of_day", "");
	}
	using namespace std::placeholders;
	
	short page = get_ran(1,0,ResMgr::strings.get("tips")->size() - 51);
	
	set_cursor(sword_curs);
	
	cDialog tips(*ResMgr::dialogs.get("tip-of-day"));
	tips.attachClickHandlers(std::bind(tip_of_day_event_filter, _1, _2, std::ref(page)),{"done","next"});
	
	tips["tip"].setText(get_str("tips",50 + page));
	
	cLed& showAtStart = dynamic_cast<cLed&>(tips["onstart"]);
	showAtStart.setState(get_bool_pref("GiveIntroHint", true) ? led_red : led_off);
	
	tips.run();
	set_pref("GiveIntroHint", showAtStart.getState() != led_off);
	save_prefs();
	
}

class cChooseScenario {
	cDialog me{*ResMgr::dialogs.get("pick-scenario")};
	std::vector<scen_header_type> scen_headers;
	void put_scen_info() {
		std::ostringstream sout;
		static const char *difficulty[] = {"Low","Medium","High","Very High"};
		auto& stk = dynamic_cast<cStack&>(me["list"]);
		stk.setPageCount(1 + ceil(scen_headers.size() / 3.0));
		
		for(size_t n = 1; n < stk.getPageCount(); n++) {
			short page = n - 1;
			stk.setPage(n);
			for(short i = 0; i < 3; i++) {
				clear_sstr(sout);
				sout << i + 1;
				std::string n = sout.str();
				if(scen_headers.size() > (page * 3 + i)) {
					me["pic" + n].show();
					dynamic_cast<cPict&>(me["pic" + n]).setPict(scen_headers[page * 3 + i].intro_pic);
					clear_sstr(sout);
					sout << scen_headers[page * 3 + i].name;
					sout << " v" << int(scen_headers[page * 3 + i].ver[0]);
					sout << '.' << int(scen_headers[page * 3 + i].ver[1]);
					sout << '.' << int(scen_headers[page * 3 + i].ver[2]);
					sout << " - |  Difficulty: " << difficulty[scen_headers[page * 3 + i].difficulty];
					sout << ", Rating: " << scen_headers[page * 3 + i].rating;
					sout << " |" << scen_headers[page * 3 + i].teaser1;
					sout << " |" << scen_headers[page * 3 + i].teaser2;
					me["desc" + n].setText(sout.str());
					me["start" + n].show();
				} else {
					me["pic" + n].hide();
					me["desc" + n].setText("");
					me["start" + n].hide();
				}
			}
		}
		stk.setPage(0);

		// Show/hide alphabetical buttons
		me["core"].hide();
		if(scen_headers.size() > 0){
			me["core"].show();
		}
		for(int i = 0; i < 26; ++i){
			me[std::string(1, (char)('a' + i))].hide();
		}
		for(auto& hdr : scen_headers){
			// I just checked, and the scenario editor will let you name your scenario "" or " "!
			std::string name = name_alphabetical(hdr.name);
			if(!name.empty()){
				// Starts with a letter:
				if(me.hasControl(name.substr(0, 1))){
					me[name.substr(0, 1)].show();
				}
				// Starts with a digit:
				else if(name[0] >= '0' && name[0] <= '9'){
					me["#"].show();
				}
			}
		}
	}

	bool doCancel() {
		me.toast(false);
		return true;
	}
	
	bool changeSelectedPage(int dir) {
		auto& stk = dynamic_cast<cStack&>(me["list"]);
		stk.changeSelectedPage(dir, true);
		return true;
	}
	
	bool doSelectScenario(int which) {
		int page = dynamic_cast<cStack&>(me["list"]).getPage();
		if(page == 0) {
			scen_header_type prefab;
			switch(which) {
				case 0: prefab.file = "valleydy.boes"; break;
				case 1: prefab.file = "stealth.boes"; break;
				case 2: prefab.file = "zakhazi.boes"; break;
				case 3: prefab.file = "busywork.boes"; break;
			}
			// Assume prefabs have the right version, I guess
			prefab.prog_make_ver[0] = 2;
			prefab.prog_make_ver[1] = 0;
			prefab.prog_make_ver[2] = 0;
			me.setResult<scen_header_type>(prefab);
		} else {
			int scen_hit = which + (page - 1) * 3;
			if(scen_hit >= scen_headers.size()) return false;
			me.setResult<scen_header_type>(scen_headers[scen_hit]);
		}
		me.toast(true);
		return true;
	}

	// Show the custom scenario folder
	bool showFolder() {
		extern fs::path scenDir;
		launchURL("file://" + scenDir.string());
		return true;
	}

	// Refresh the scenario list (check if custom scenarios added/removed)
	bool refreshList() {
		scen_headers = build_scen_headers();
		auto& stk = dynamic_cast<cStack&>(me["list"]);
		short page = stk.getPage();
		// Redo the stack
		put_scen_info();
		// Put the viewer to the same page it was (clamped in case the list got shorter)
		stk.setPage(min(page, stk.getPageCount() - 1));
		return true;
	}
public:
	cChooseScenario() {
		scen_headers = build_scen_headers();
	}
	scen_header_type run() {
		using namespace std::placeholders;
		extern fs::path scenDir;
		
		set_cursor(sword_curs);
		
		me["cancel"].attachClickHandler(std::bind(&cChooseScenario::doCancel, this));
		me["next"].attachClickHandler(std::bind(&cChooseScenario::changeSelectedPage, this, 1));
		me["prev"].attachClickHandler(std::bind(&cChooseScenario::changeSelectedPage, this, -1));
		me["start1"].attachClickHandler(std::bind(&cChooseScenario::doSelectScenario, this, 0));
		me["start2"].attachClickHandler(std::bind(&cChooseScenario::doSelectScenario, this, 1));
		me["start3"].attachClickHandler(std::bind(&cChooseScenario::doSelectScenario, this, 2));
		me["scen1"].attachClickHandler(std::bind(&cChooseScenario::doSelectScenario, this, 0));
		me["scen2"].attachClickHandler(std::bind(&cChooseScenario::doSelectScenario, this, 1));
		me["scen3"].attachClickHandler(std::bind(&cChooseScenario::doSelectScenario, this, 2));
		me["folder"].attachClickHandler(std::bind(&cChooseScenario::showFolder, this));
		me["refresh"].attachClickHandler(std::bind(&cChooseScenario::refreshList, this));
		me["core"].attachClickHandler([](cDialog& me, std::string, eKeyMod) -> bool {
			auto& stk = dynamic_cast<cStack&>(me["list"]);
			stk.setPage(0);
			return true;
		});
		// Letter buttons scroll to an alphabetical position:
		for(int i = 0; i < 26; ++i){
			std::string letter(1, (char)('a' + i));
			me[letter].attachClickHandler([this](cDialog& me, std::string letter, eKeyMod) -> bool {
				size_t first_letter_idx = std::find_if(scen_headers.begin(), scen_headers.end(), [letter](scen_header_type hdr) -> bool {
					std::string name = name_alphabetical(hdr.name);
					return !name.empty() && name.substr(0, 1) == letter;
				}) - scen_headers.begin();
				size_t page = 1 + first_letter_idx / 3;
				auto& stk = dynamic_cast<cStack&>(me["list"]);
				stk.setPage(page);
				return true;
			});
		}
		// Number button scrolls to scenarios that start with symbols or digits:
		me["#"].attachClickHandler([this](cDialog& me, std::string letter, eKeyMod) -> bool {
			auto& stk = dynamic_cast<cStack&>(me["list"]);
			stk.setPage(1);
			return true;
		});
		
		put_scen_info();
		
		if(scen_headers.empty()) {
			me["next"].hide();
			me["prev"].hide();
		}
		
		me.run();
		if(!me.hasResult()) return scen_header_type{};
		scen_header_type scen = me.getResult<scen_header_type>();
		if(scen.file.empty()){
			std::ostringstream error;
			error << "Scenario '" << scen.name << "' is missing!";
			throw error.str();
		}
		return scen;
	}
};

scen_header_type pick_a_scen() {
	// build_scen_headers() can be slow.
	set_cursor(watch_curs);
	cChooseScenario dlg;
	restore_cursor();
	return dlg.run();
}

extern fs::path saveDir;

// When loading an autosave, store the main save filename because univ.file should be set to the
// main save. That way, manual saves overwrite the main save, not the specific autosave you choose.
fs::path store_chose_auto;

class cFilePicker {
	const int SLOTS_PER_PAGE = 4;
	int parties_per_page;

	fs::path save_folder;
	bool picking_auto;
	bool saving;
	cDialog me;
	cStack& get_stack() { return dynamic_cast<cStack&>(me["list"]); }
	std::string template_info_str;

	std::vector<std::pair<fs::path, std::time_t>> save_file_mtimes;
	// We have to load the parties to get PC graphics, average level, location, etc.
	// But we shouldn't load them all at once, because the amount is unlimited.
	std::vector<cUniverse> save_files;
	int pages_populated = 0;
	int saves_loaded = 0;

	bool dummy_picker_chose = false;
	void init_pages() {
		save_file_mtimes = sorted_file_mtimes(save_folder);
		save_files.resize(save_file_mtimes.size());

		cStack& stk = get_stack();
		int num_pages = max(1, ceil((float)save_file_mtimes.size() / parties_per_page));
		stk.setPageCount(num_pages);
		// HACK: For some reason which should be fixed, the buttons and labels on subsequent pages
		// aren't getting text on the static buttons and labels
		std::string save_button_text = me["save1"].getText();
		std::string ext_label_text = me["file1-extension-label"].getText();
		std::string load_button_text = me["load1"].getText();
		std::string auto_button_text = me["auto1"].getText();
		std::string newer_label_text = me["auto1-more-recent"].getText();
		for(int i = 0; i < stk.getPageCount(); ++i){
			stk.setPage(i);
			if(saving){
				me["title-load"].hide();
				me["title-auto"].hide();
				me["file1"].setText(""); // Keep the frame
					me["file1-extension-label"].setText(ext_label_text);
				for(int i = 0; i < SLOTS_PER_PAGE; ++i){
					me["save" + std::to_string(i+1)].setText(save_button_text);
					me["load" + std::to_string(i+1)].hide();
				}
			}else{
				if(picking_auto){
					me["title-load"].hide();
					fs::path party_name = save_folder.filename();
					party_name.replace_extension();
					me["title-auto"].replaceText("{Folder}", party_name.string());
				}else{
					me["title-auto"].hide();
				}
				me["title-save"].hide();
				me["file1-field"].hide();
				me["file1-extension-label"].hide();
				for(int i = 0; i < SLOTS_PER_PAGE; ++i){
					me["save" + std::to_string(i+1)].hide();
					me["load" + std::to_string(i+1)].setText(load_button_text);
					me["auto" + std::to_string(i+1)].setText(auto_button_text);
					me["auto" + std::to_string(i+1) + "-more-recent"].setText(newer_label_text);
				}
			}
		}
		stk.setPage(0);
	}

	void empty_slot(int idx) {
		std::string suffix = std::to_string(idx+1);
		me["file" + suffix].setText("");
		me["pc" + suffix + "a"].hide();
		me["pc" + suffix + "b"].hide();
		me["pc" + suffix + "c"].hide();
		me["pc" + suffix + "d"].hide();
		me["pc" + suffix + "e"].hide();
		me["pc" + suffix + "f"].hide();
		me["info" + suffix].hide();
		me["load" + suffix].hide();
		me["auto" + suffix].hide();
		me["auto" + suffix + "-more-recent"].hide();
	}

	void dummy_slot(int idx) {
		std::string suffix = std::to_string(idx+1);
		me["file" + suffix].setText("<Replay placeholder>");
		me["pc" + suffix + "a"].hide();
		me["pc" + suffix + "b"].hide();
		me["pc" + suffix + "c"].hide();
		me["pc" + suffix + "d"].hide();
		me["pc" + suffix + "e"].hide();
		me["pc" + suffix + "f"].hide();
		me["info" + suffix].hide();
		me["auto" + suffix + "-more-recent"].hide();
	}

	void corrupt_slot(int idx) {
		std::string suffix = std::to_string(idx+1);
		me["pc" + suffix + "a"].hide();
		me["pc" + suffix + "b"].hide();
		me["pc" + suffix + "c"].hide();
		me["pc" + suffix + "d"].hide();
		me["pc" + suffix + "e"].hide();
		me["pc" + suffix + "f"].hide();
		me["info" + suffix].setText("This file is corrupt and cannot be opened.");
		me["save" + suffix].hide();
		me["load" + suffix].hide();
		me["info" + suffix].show();
		me["auto" + suffix + "-more-recent"].hide();
		me["auto" + suffix].hide();
	}

	void populate_slot(int idx, fs::path file, std::time_t mtime, cUniverse& party_univ) {
		if(replaying){
			dummy_slot(idx);
			return;
		}
		std::string suffix = std::to_string(idx+1);
		me["file" + suffix].setText(file.filename().string());
		if(party_univ.party.load_failed){
			corrupt_slot(idx);
			return;
		}

		// Populate PC graphics
		for(int i = 0; i < 6; ++i){
			std::string key = "pc" + suffix;
			key.push_back((char)('a' + i));
			cPict& pict = dynamic_cast<cPict&>(me[key]);
			if(party_univ.party[i].main_status != eMainStatus::ABSENT) {
				pic_num_t pic = party_univ.party[i].which_graphic;
				// TODO Apparently PCs are supposed to be able to have custom graphics and monster graphics,
				// but the special node to Create PC only allows choosing preset PC pictures, so I don't
				// think it's even possible to get a non-preset-PC graphic into a party without directly
				// editing the tagfile. For now, I'm only rendering preset PCs.
				if(pic >= 1000){
				}else if(pic >= 100){
				}else{
					pict.setPict(pic, PIC_PC);
				}
				pict.show();
			}else{
				pict.hide();
			}
		}

		// Populate party info
		std::string party_info = template_info_str;

		short level_sum = 0;
		short num_pc = 0;
		for(int i = 0; i < 6; ++i){
			if(party_univ.party[i].main_status != eMainStatus::ABSENT) {
				level_sum += party_univ.party[i].level;
				++num_pc;
			}
		}
		short avg_level = round((float)(level_sum / num_pc));
		boost::replace_first(party_info, "{Lv}", std::to_string(avg_level));
		if(num_pc == 1){
			boost::replace_first(party_info, "Avg. ", "");
		}

		auto local_time = *std::localtime(&mtime);
		std::stringstream last_modified;
		last_modified << std::put_time(&local_time, "%h %d, %Y %I:%M %p");
		boost::replace_first(party_info, "{LastSaved}", last_modified.str());

		if(!party_univ.party.scen_name.empty()){
			boost::replace_first(party_info, "{Scenario}", party_univ.scenario.scen_name);
			boost::replace_first(party_info, "{Location}", get_location(&party_univ));
		}else{
			boost::replace_first(party_info, "{Scenario}||{Location}", "");
		}

		me["info" + suffix].setText(party_info);

		// Set up buttons
		if(saving){
			me["file1"].setText(""); // Keep the frame
			me["auto" + suffix].hide();
			me["auto" + suffix + "-more-recent"].hide();
			me["save" + suffix].attachClickHandler(std::bind(&cFilePicker::doSave, this, file));
		}else{
			me["load" + suffix].attachClickHandler(std::bind(&cFilePicker::doLoad, this, file));

			std::vector<std::pair<fs::path, std::time_t>> auto_mtimes;
			fs::path auto_folder = file;
			if(!picking_auto){
				auto_folder.replace_extension(".auto");
				if(fs::is_directory(auto_folder)) auto_mtimes = sorted_file_mtimes(auto_folder);
			}

			me["auto" + suffix + "-more-recent"].hide();
			if(auto_mtimes.empty()){
				me["auto" + suffix].hide();
			}else{
				// If an autosave is newer than the main save, show an indicator
				if(std::difftime(mtime, auto_mtimes.front().second) < 0)
					me["auto" + suffix + "-more-recent"].show();

				me["auto" + suffix].attachClickHandler(std::bind(&cFilePicker::showAuto, this, auto_folder, file));
			}
		}
	}

	void populate_page(int page) {
		int parties_needed = min(save_file_mtimes.size(), (page+1) * parties_per_page);
		set_cursor(watch_curs);
		while(saves_loaded < parties_needed){
			fs::path next_file = save_file_mtimes[saves_loaded].first;
			cUniverse party_univ;
			cCustomGraphics graphics;
			if(!load_party(next_file, save_files[saves_loaded], graphics, true)){
				// Below, we check the load_failed flag to display when a party is corrupt
			}
			saves_loaded++;
		}
		restore_cursor();

		if(saving){
			time_t now;
			std::time(&now);
			// Populate the first slot with the actual current party
			populate_slot(0, "", now, univ);
		}

		int start_idx = page * parties_per_page;
		for(int party_idx = start_idx; party_idx < start_idx + parties_per_page; ++party_idx){
			int slot_idx = party_idx - start_idx;
			if(saving) slot_idx++;
			if(party_idx < parties_needed)
				populate_slot(slot_idx, save_file_mtimes[party_idx].first, save_file_mtimes[party_idx].second, save_files[party_idx]);
			else
				empty_slot(slot_idx);
		}

		++pages_populated;
	}

	bool showAuto(fs::path auto_folder, fs::path file) {
		fs::path autosave = run_autosave_picker(auto_folder, &me);
		if(!autosave.empty()){
			store_chose_auto = file;
			doLoad(autosave);
		}
		return true;
	}

	bool dummyShowAuto(fs::path auto_folder, fs::path file) {
		cFilePicker dummyAuto(auto_folder, false, &me, true);
		dummyAuto.run();
		if(dummyAuto.dummy_picker_chose){
			doCloseDummyPicker();
		}
		return true;
	}

	bool doLoad(fs::path selected_file) {
		me.setResult(selected_file);
		me.toast(false);
		return true;
	}

	bool confirm_overwrite(fs::path selected_file) {
		cChoiceDlog dlog("confirm-overwrite", {"save","cancel"}, &me);
		cDialog& inner = *(dlog.operator->());
		std::string prompt = inner["prompt"].getText();
		boost::replace_first(prompt, "{File}", selected_file.filename().string());
		inner["prompt"].setText(prompt);
		std::string choice = dlog.show();
		return choice == "save";
	}

	bool doSave(fs::path selected_file) {
		if(selected_file.empty()){
			selected_file = save_folder / me["file1-field"].getText();
			selected_file += ".exg";
		}
		if(!fs::exists(selected_file) || confirm_overwrite(selected_file)){
			me.setResult(selected_file);
			me.toast(false);
		}
		return true;
	}

	bool doCloseDummyPicker() {
		fs::path p {""};
		me.setResult(p);
		dummy_picker_chose = true;
		me.toast(false);
		return true;
	}

	bool doCancel() {
		me.toast(false);
		return true;
	}

	bool changeSelectedPage(int dir) {
		auto& stk = dynamic_cast<cStack&>(me["list"]);
		cControl::storage_t field_storage = me["file1-field"].store();
		// This stack doesn't loop. It's easier to implement loading the files one page at a time
		// if I know we're not gonna jump from page 0 to the last page, leaving a gap in the vector.
		stk.changeSelectedPage(dir);
		me["file1-field"].restore(field_storage);
		me["prev"].show();
		me["next"].show();
		if(stk.getPage() == 0){
			me["prev"].hide();
		}
		if(stk.getPage() == stk.getPageCount() - 1){
			me["next"].hide();
		}

		populate_page(stk.getPage());
		return true;
	}

	bool doFileBrowser() {
		fs::path from_browser = "";
		if(replaying){
			if(has_next_action("load_party")){
				from_browser = "DUMMY";
			}
		}else{
			from_browser = nav_get_party();
		}
		if(!from_browser.empty()){
			me.setResult(from_browser);
			me.toast(false);
		}
		return true;
	}

public:
	cFilePicker(fs::path save_folder, bool saving, cDialog* parent = nullptr, bool picking_auto = false) :
		me(*ResMgr::dialogs.get("pick-save"), parent),
		save_folder(save_folder),
		picking_auto(picking_auto),
		saving(saving),
		parties_per_page(saving ? SLOTS_PER_PAGE - 1 : SLOTS_PER_PAGE) {}

	fs::path run() {
		template_info_str = me["info1"].getText();

		// When doing Save As, make Enter save in a new file, not cancel:
		if(saving){
			me["cancel"].setDefault(false);
			me["save1"].setDefault(true);
		}

		me["cancel"].attachClickHandler(std::bind(&cFilePicker::doCancel, this));
		me["find"].attachClickHandler(std::bind(&cFilePicker::doFileBrowser, this));
		// Since it would be crazy to record and replay the metadata shown on a player's save picker
		// dialog (which is what we do for the scenario picker),
		// when replaying, basically make Left/Right buttons no-op.
		// Load/Save buttons should send a dummy result.
		if(!replaying){
			me["next"].attachClickHandler(std::bind(&cFilePicker::changeSelectedPage, this, 1));
			me["prev"].attachClickHandler(std::bind(&cFilePicker::changeSelectedPage, this, -1));
			init_pages();
		}else{
			for(int i = 0; i < SLOTS_PER_PAGE; ++i){
				std::string suffix = std::to_string(i+1);
				// When replaying, a click on a load or save button means the dummy file picker can go away:
				me["load" + suffix].attachClickHandler(std::bind(&cFilePicker::doCloseDummyPicker, this));
				me["save" + suffix].attachClickHandler(std::bind(&cFilePicker::doCloseDummyPicker, this));
				// A click on an autosave button means another dummy file picker should open:
				me["auto" + suffix].attachClickHandler(std::bind(&cFilePicker::dummyShowAuto, this, "", ""));
			}
		}

		// Hide the prev button and populate the first page
		changeSelectedPage(0);

		me.run();
		if(!me.hasResult()) return "";
		fs::path file = me.getResult<fs::path>();
		return file;
	}
};

static fs::path run_file_picker(fs::path save_folder, bool saving) {
	return cFilePicker(save_folder, saving).run();
}

fs::path run_autosave_picker(fs::path auto_folder, cDialog* parent) {
	return cFilePicker(auto_folder, false, parent, true).run();
}

fs::path fancy_file_picker(bool saving) {
	if(recording){
		record_action("fancy_file_picker", bool_to_str(saving));
	}
	// TODO this is set up to be configurable, but not yet exposed in preferences.
	fs::path save_folder = get_string_pref("SaveFolder", saveDir.string());

	return run_file_picker(save_folder, saving);
}

fs::path run_file_picker(bool saving){
	if(has_feature_flag("file-picker-dialog", "V1") && get_bool_pref("FancyFilePicker", true)){
		store_chose_auto = "";
		return fancy_file_picker(saving);
	}

	return os_file_picker(saving);
}
