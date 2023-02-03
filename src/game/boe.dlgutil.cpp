
#include <cstring>

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

extern eItemWinMode stat_window;
extern eGameMode overall_mode;
extern bool changed_display_mode;
extern sf::RenderWindow mainPtr;
extern rectangle d_rects[80];
extern short d_rect_index[80];
extern eStatMode stat_screen_mode;
extern long register_flag;
extern long ed_flag,ed_key;
extern location center;
extern std::shared_ptr<cScrollbar> text_sbar,item_sbar,shop_sbar;
extern std::shared_ptr<cButton> done_btn, help_btn;
extern bool map_visible;
extern sf::RenderWindow mini_map;
extern cUniverse univ;
extern sf::Texture pc_gworld;
extern std::map<eSkill,short> skill_max;

short sign_mode,person_graphic,store_person_graphic,store_sign_mode;
long num_talk_entries;
char null_string[256] = "";
extern bool party_in_memory;

// Talking vars
eGameMode store_pre_talk_mode;
short store_personality,store_personality_graphic,shop_identify_cost;
std::string save_talk_str1, save_talk_str2;
sf::RenderTexture talk_gworld;
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

void start_shop_mode(short which,short cost_adj,std::string store_name) {
	rectangle area_rect;
	if(which < 0 || which >= univ.scenario.shops.size()) {
		showError("The scenario tried to place you in a nonexistent shop!");
		return;
	}
	
	// This would be a place to hide the text box, if I add it.
	
	active_shop_num = which;
	active_shop = univ.scenario.shops[which];
	active_shop.setCostAdjust(cost_adj);
	active_shop.setName(store_name);
	
	area_rect = talk_area_rect;
	talk_gworld.create(area_rect.width(), area_rect.height());
	
	store_pre_shop_mode = overall_mode;
	overall_mode = MODE_SHOPPING;
	stat_screen_mode = MODE_SHOP;
	shop_sbar->setPosition(0);
	
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
	put_background();
	
	draw_shop_graphics(0,area_rect);
	
	put_item_screen(stat_window);
	give_help(26,27);
}

static void update_last_talk(int new_node) {
	// Store last node in the Go Back button
	for(word_rect_t& word : talk_words) {
		if(word.word != "Go Back") continue;
		word.node = current_talk_node;
		current_talk_node = new_node;
		break;
	}
}

void end_shop_mode() {
	rectangle dummy_rect = {0,0,0,0};
	
	// This would be a place to show the text box, if I add it (and if this is not an outdoor shop).
	
	shop_sbar->hide();
	done_btn->hide();
	help_btn->hide();
	if(store_pre_shop_mode == MODE_TALKING) {
		place_talk_str("You conclude your business.", "", 0, dummy_rect);
		update_last_talk(TALK_BUSINESS);
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
}

void handle_shop_event(location p) {
	if(p.in(talk_help_rect)) {
		if(!help_btn->handleClick(p))
			return;
		give_help(226,27);
		return;
	}
	
	if(p.in(shop_done_rect)) {
		if(done_btn->handleClick(p))
			end_shop_mode();
		return;
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
		active_rect.offset(talk_gword_offset_x, talk_gword_offset_y);

		rectangle item_help_rect { shopping_rects[i][SHOPRECT_ITEM_HELP] };
		item_help_rect.offset(talk_gword_offset_x, talk_gword_offset_y);

		if(p.in(active_rect)) {
			click_shop_rect(active_rect);
			handle_sale(active_shop.getItem(what_picked), what_picked);
			set_up_shop_array();
			draw_shop_graphics(false, {});
		} else if(p.in(item_help_rect)){
			click_shop_rect(item_help_rect);
			handle_info_request(active_shop.getItem(what_picked));
		}
	}
}

void handle_sale(cShopItem item, int i) {
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
			}
			break;
		case eShopItemType::ALCHEMY:
			if(univ.party.alchemy[base_item.item_level])
				ASB("You already know that recipe.");
			else if(!take_gold(cost,false))
				ASB("Not enough gold.");
			else {
				// TODO: This seems like the wrong sound
				play_sound(62);
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
						for(int i = 0; i < univ.current_pc().items.size(); i++)
							if((univ.current_pc().equip[i]) &&
								(univ.current_pc().items[i].cursed))
								univ.current_pc().items[i].cursed = univ.current_pc().items[i].unsellable = false;
						break;
					case eShopItemType::DESTONE: case eShopItemType::RAISE_DEAD: case eShopItemType::RESURRECT:
						univ.current_pc().main_status = eMainStatus::ALIVE;
						break;
					case eShopItemType::CURE_DUMBFOUNDING:
						// TODO: Don't cure anti-dumbfounding!
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
				// TODO: This seems like the wrong sound
				play_sound(62);
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
				// TODO: This seems like the wrong sound
				play_sound(62);
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
				// TODO: No idea what's a good sound here.
				play_sound(62);
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
	draw_shop_graphics(0,dummy_rect);
	print_buf();
	put_pc_screen();
	put_item_screen(stat_window);
}


void handle_info_request(cShopItem item) {
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
			display_spells(eSkill::MAGE_SPELLS,base_item.item_level,0);
			break;
		case eShopItemType::PRIEST_SPELL:
			display_spells(eSkill::PRIEST_SPELLS,base_item.item_level,0);
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
				for(int i = 0; i < univ.current_pc().items.size(); i++) {
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

void start_talk_mode(short m_num,short personality,mon_num_t monst_type,short store_face_pic) {
	rectangle area_rect;
	
	store_personality = personality;
	
	store_monst_type = monst_type;
	store_m_num = m_num;
	store_talk_face_pic = store_face_pic; ////
	area_rect = talk_area_rect;
	talk_gworld.create(area_rect.width(), area_rect.height());
	help_btn->show();
	
	// This would be the place to show the text box, if I add it.
	
	// Set the current town for talk strings
	univ.town.prep_talk(personality / 10);
	
	// Dredge up critter's name
	title_string = std::string(univ.town.cur_talk().people[personality % 10].title) + ":";
	
	store_pre_talk_mode = overall_mode;
	overall_mode = MODE_TALKING;
	talk_end_forced = false;
	
	// first initialise talk_words here
	talk_words.clear();
	static const rectangle preset_rects[9] = {
		rectangle{366,4,386,54}, rectangle{366,70,386,130}, rectangle{366,136,386,186},
		rectangle{389,4,409,54}, rectangle{389,70,409,120}, rectangle{389,121,409,186},
		rectangle{389,210,409,270}, rectangle{366,190,386,270},
		rectangle{343,4,363,134},
	};
	static const char*const preset_words[9] = {
		"Look", "Name", "Job",
		"Buy", "Sell", "Record",
		"Done", "Go Back",
		"Ask About...",
	};
	
	// Place buttons at bottom.
	for(short i = 0; i < 9; i++) {
		word_rect_t preset_word(preset_words[i], preset_rects[i]);
		preset_word.on = Colours::DARK_GREEN;
		preset_word.off = Colours::LIGHT_GREEN;
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
	
	stat_screen_mode = MODE_SHOP;
	current_talk_node = TALK_LOOK;
	
	// Bring up and place first strings.
	save_talk_str1 = univ.town.cur_talk().people[personality % 10].look;
	save_talk_str2 = "";
	can_save_talk = true;
	
	place_talk_str(save_talk_str1, "", 0, dummy_rect);
	
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

void handle_talk_event(location p) {
	short get_pc,s1 = -1,s2 = -1;
	char asked[4];
	
	short a,b,c,d;
	eTalkNode ttype;
	
	if(p.in(talk_help_rect)) {
		if(!help_btn->handleClick(p))
			return;
		give_help(205,6);
		return;
	}
	
	p.x -= 5;
	p.y -= 5;
	
	int which_talk_entry = TALK_DUNNO;
	for(word_rect_t& word : talk_words) {
		if(word.node == -1) continue;
		rectangle wordRect(word.rect);
		wordRect.offset(talk_area_rect.topLeft());
		wordRect.offset(-1, -10); // TODO: This corrects for the byzantine offsets that win_draw_string applies for some reason...
		if(!p.in(wordRect)) continue;
		click_talk_rect(word);
		which_talk_entry = word.node;
		break;
	}
	if(which_talk_entry == TALK_DUNNO)
		return;
	
	switch(which_talk_entry) {
		case TALK_DUNNO:
		SPECIAL_DUNNO:
			save_talk_str1 = univ.town.cur_talk().people[store_personality % 10].dunno;
			if(save_talk_str1.length() < 2) save_talk_str1 = "You get no response.";
			place_talk_str(save_talk_str1, "", 0, dummy_rect);
			update_last_talk(TALK_DUNNO);
			return;
		case TALK_BUSINESS: // This one only reachable via "go back".
			place_talk_str("You conclude your business.", "", 0, dummy_rect);
			update_last_talk(TALK_BUSINESS);
			return;
		case TALK_LOOK:
		SPECIAL_LOOK:
			save_talk_str1 = univ.town.cur_talk().people[store_personality % 10].look;
			place_talk_str(save_talk_str1, "", 0, dummy_rect);
			update_last_talk(TALK_LOOK);
			return;
		case TALK_NAME:
		SPECIAL_NAME:
			save_talk_str1 = univ.town.cur_talk().people[store_personality % 10].name;
			place_talk_str(save_talk_str1, "", 0, dummy_rect);
			update_last_talk(TALK_NAME);
			return;
		case TALK_JOB:
		SPECIAL_JOB:
			save_talk_str1 = univ.town.cur_talk().people[store_personality % 10].job;
			place_talk_str(save_talk_str1, "", 0, dummy_rect);
			update_last_talk(TALK_JOB);
			return;
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
			if(!can_save_talk) {
				beep();
				return;
			}
			if(univ.party.save_talk(univ.town->talking.people[store_personality%10].title, univ.town->name, save_talk_str1, save_talk_str2)) {
				give_help(57,0);
				play_sound(0);
				ASB("Noted in journal.");
			} else ASB("This is already saved.");
			print_buf();
			return;
		case TALK_DONE:
		SPECIAL_DONE:
			end_talk_mode();
			return;
		case TALK_BACK: // only if there's nothing to go back to
			return; // so, there's nothing to do here
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
			if((get_pc = char_select_pc(0,"Train who?")) < 6) {
				can_save_talk = false;
				spend_xp(get_pc,1, nullptr);
			}
			save_talk_str1 = "You conclude your training.";
			return;
			
		case eTalkNode::SHOP:
			if(d < 0 || d > 11) {
				showError("Invalid shop type!");
				return;
			}
			start_shop_mode(b,a,save_talk_str1);
			can_save_talk = false;
			return;
		case eTalkNode::JOB_BANK:
			if(a < univ.party.job_banks.size() && univ.party.job_banks[a].anger >= 50) {
				save_talk_str1 = save_talk_str2;
				save_talk_str2 = "";
				break;
			} else {
				show_job_bank(a, save_talk_str1.c_str());
				return;
			}
		case eTalkNode::SELL_WEAPONS:
			can_save_talk = false;
			stat_screen_mode = MODE_SELL_WEAP;
			put_item_screen(stat_window);
			give_help(42,43);
			break;
		case eTalkNode::SELL_ARMOR:
			can_save_talk = false;
			stat_screen_mode = MODE_SELL_ARMOR;
			put_item_screen(stat_window);
			give_help(42,43);
			break;
		case eTalkNode::SELL_ITEMS:
			can_save_talk = false;
			stat_screen_mode = MODE_SELL_ANY;
			put_item_screen(stat_window);
			give_help(42,43);
			break;
		case eTalkNode::IDENTIFY: case eTalkNode::ENCHANT:
			can_save_talk = false;
			stat_screen_mode = (ttype == eTalkNode::IDENTIFY) ? MODE_IDENTIFY : MODE_ENCHANT;
			shop_identify_cost = a;
			put_item_screen(stat_window);
			give_help(ttype == eTalkNode::IDENTIFY ? 44 : 45,0);
			break;
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
				save_talk_str1 + save_talk_str2;
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
					save_talk_str2 = "";
				} else {
					save_talk_str1 = "There are no horses left.";
					save_talk_str2 = "";
					can_save_talk = false;
				}
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
				// TODO: Uh, is something supposed to happen here?
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
				univ.town.monst[store_m_num].active = 0;
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
			get_strs(save_talk_str1, save_talk_str2, eSpecCtxType::TOWN, s1, s2);
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
			get_strs(save_talk_str1, save_talk_str2, eSpecCtxType::SCEN, s1, s2);
			put_pc_screen();
			put_item_screen(stat_window);
			break;
	}
	
	place_talk_str(save_talk_str1,save_talk_str2,0,dummy_rect);
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
	
	if(town_num >= 200) {
		town_num -= 200;
		sign_text = univ.out->sign_locs[which_sign].text;
	}
	else {
		sign_text = univ.town->sign_locs[which_sign].text;
	}
	sign->getControl("sign").setText(sign_text);
	
	sign.show();
}

void save_prefs(bool resetHelp){
	
	if(resetHelp) clear_pref("ReceivedHelp");
	
	bool success = sync_prefs();
	if(!success){
		showWarning("There was a problem writing to the preferences file. When the game is next run the preferences will revert to their previously set values.","Should you manage to resolve the problem without closing the program, simply open the preferences screen and click \"OK\" to try again.");
	}
}

static bool prefs_event_filter (cDialog& me, std::string id, eKeyMod) {
	bool did_cancel = false,reset_help = false;
	
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
		set_pref("DrawTerrainFrills", dynamic_cast<cLed&>(me["nofrills"]).getState() == led_off);
		set_pref("RepeatRoomDescriptions", dynamic_cast<cLed&>(me["repeatdesc"]).getState() != led_off);
		set_pref("ShowInstantHelp", dynamic_cast<cLed&>(me["nohelp"]).getState() == led_off);
		
		if(overall_mode == MODE_STARTUP && !party_in_memory) {
			set_pref("EasyMode", dynamic_cast<cLed&>(me["easier"]).getState() != led_off);
			set_pref("LessWanderingMonsters", dynamic_cast<cLed&>(me["lesswm"]).getState() != led_off);
		} else {
			univ.party.easy_mode = dynamic_cast<cLed&>(me["easier"]).getState() != led_off;
			univ.party.less_wm = dynamic_cast<cLed&>(me["lesswm"]).getState() != led_off;
		}
		set_pref("DrawTerrainAnimation", dynamic_cast<cLed&>(me["noanim"]).getState() == led_off);
		set_pref("DrawTerrainShoreFrills", dynamic_cast<cLed&>(me["noshore"]).getState() == led_off);
		set_pref("ShowStartupSplash", dynamic_cast<cLed&>(me["skipsplash"]).getState() == led_off);
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
		if(dynamic_cast<cLed&>(me["resethelp"]).getState() == led_red) {
			reset_help = true;
		}
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
	save_prefs(reset_help);
	return true;
}

void pick_preferences() {
	set_cursor(sword_curs);
	
	cDialog prefsDlog(*ResMgr::dialogs.get("preferences"));
	prefsDlog.attachClickHandlers(&prefs_event_filter, {"okay", "cancel"});
	
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
	dynamic_cast<cLed&>(prefsDlog["nofrills"]).setState(get_bool_pref("DrawTerrainFrills", true) ? led_off : led_red);
	dynamic_cast<cLed&>(prefsDlog["repeatdesc"]).setState(get_bool_pref("RepeatRoomDescriptions") ? led_red : led_off);
	dynamic_cast<cLed&>(prefsDlog["nohelp"]).setState(get_bool_pref("ShowInstantHelp", true) ? led_off : led_red);
	if(overall_mode == MODE_STARTUP && !party_in_memory) {
		dynamic_cast<cLed&>(prefsDlog["easier"]).setState(get_bool_pref("EasyMode") ? led_red : led_off);
		dynamic_cast<cLed&>(prefsDlog["lesswm"]).setState(get_bool_pref("LessWanderingMonsters") ? led_red : led_off);
	} else {
		dynamic_cast<cLed&>(prefsDlog["easier"]).setState(univ.party.easy_mode ? led_red : led_off);
		dynamic_cast<cLed&>(prefsDlog["lesswm"]).setState(univ.party.less_wm ? led_red : led_off);
	}
	dynamic_cast<cLed&>(prefsDlog["noanim"]).setState(get_bool_pref("DrawTerrainAnimations", true) ? led_off : led_red);
	dynamic_cast<cLed&>(prefsDlog["noshore"]).setState(get_bool_pref("DrawTerrainShoreFrills", true) ? led_off : led_red);
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
	
	cLedGroup& uiScale = dynamic_cast<cLedGroup&>(prefsDlog["scaleui"]);
	double ui_scale = get_float_pref("UIScale", 1.0);
	if (ui_scale>0.95 && ui_scale<1.05) uiScale.setSelected("1");
	else if (ui_scale>1.45 && ui_scale<1.55) uiScale.setSelected("1_5");
	else if (ui_scale>1.95 && ui_scale<2.05) uiScale.setSelected("2");
	else if (ui_scale>2.95 && ui_scale<3.05) uiScale.setSelected("3");
	else if (ui_scale>3.95 && ui_scale<4.05) uiScale.setSelected("4");
	else uiScale.setSelected("other");

	cLedGroup& uiMapScale = dynamic_cast<cLedGroup&>(prefsDlog["scalemap"]);
	double ui_map_scale = get_float_pref("UIScaleMap", 1.0);
	if (ui_map_scale>0.95 && ui_map_scale<1.05) uiMapScale.setSelected("1");
	else if (ui_map_scale>1.45 && ui_map_scale<1.55) uiMapScale.setSelected("1_5");
	else if (ui_map_scale>1.95 && ui_map_scale<2.05) uiMapScale.setSelected("2");
	else if (ui_map_scale>2.95 && ui_map_scale<3.05) uiMapScale.setSelected("3");
	else if (ui_map_scale>3.95 && ui_map_scale<4.05) uiMapScale.setSelected("4");
	else uiMapScale.setSelected("other");
	
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

	void (*give_help)(short,short,cDialog&) = ::give_help;
	
	int store_display_mode = get_int_pref("DisplayMode");
	prefsDlog.run(std::bind(give_help, 55, 0, std::ref(prefsDlog)));
	
	if(get_int_pref("DisplayMode") != store_display_mode || get_float_pref("UIScale") != ui_scale || get_float_pref("UIScaleMap") != ui_map_scale)
		changed_display_mode = true;
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
		give_help(222,23,me);
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
	void (*give_help)(short,short,cDialog&) = ::give_help;
	
	pcDialog.run(std::bind(give_help, 22, 23, std::ref(pcDialog)));
	
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
				sout.clear();
				sout.str("");
				sout << i + 1;
				std::string n = sout.str();
				if(scen_headers.size() > (page * 3 + i)) {
					me["pic" + n].show();
					dynamic_cast<cPict&>(me["pic" + n]).setPict(scen_headers[page * 3 + i].intro_pic);
					sout.str("");
					sout << scen_headers[page * 3 + i].name;
					sout << " v" << int(scen_headers[page * 3 + i].ver[0]);
					sout << '.' << int(scen_headers[page * 3 + i].ver[1]);
					sout << '.' << int(scen_headers[page * 3 + i].ver[2]);
					sout << " - |  Difficulty: " << difficulty[scen_headers[page * 3 + i].difficulty];
					sout << ", Rating: " << scen_headers[page * 3 + i].rating;
					sout << " |" << scen_headers[page * 3 + i].who1;
					sout << " |" << scen_headers[page * 3 + i].who2;
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
	}

	bool doCancel() {
		scen_header_type null;
		me.setResult<scen_header_type>(null);
		me.toast(false);
		return true;
	}
	
	bool doSelectPage(int dir) {
		auto& stk = dynamic_cast<cStack&>(me["list"]);
		int curPage = stk.getPage(), nPages = stk.getPageCount();
		curPage += dir;
		if(curPage < 0) curPage += nPages;
		else if(curPage >= nPages) curPage -= nPages;
		stk.setPage(curPage);
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
public:
	cChooseScenario() {
		// TODO: Add a button to jump to the scenarios folder
		scen_headers = build_scen_headers(); // TODO: Either make this local to this class, or make it take scen_headers by reference
	}
	scen_header_type run() {
		using namespace std::placeholders;
		extern fs::path scenDir;
		
		set_cursor(sword_curs);
		
		me["cancel"].attachClickHandler(std::bind(&cChooseScenario::doCancel, this));
		me["next"].attachClickHandler(std::bind(&cChooseScenario::doSelectPage, this, 1));
		me["prev"].attachClickHandler(std::bind(&cChooseScenario::doSelectPage, this, -1));
		me["start1"].attachClickHandler(std::bind(&cChooseScenario::doSelectScenario, this, 0));
		me["start2"].attachClickHandler(std::bind(&cChooseScenario::doSelectScenario, this, 1));
		me["start3"].attachClickHandler(std::bind(&cChooseScenario::doSelectScenario, this, 2));
		me["scen1"].attachClickHandler(std::bind(&cChooseScenario::doSelectScenario, this, 0));
		me["scen2"].attachClickHandler(std::bind(&cChooseScenario::doSelectScenario, this, 1));
		me["scen3"].attachClickHandler(std::bind(&cChooseScenario::doSelectScenario, this, 2));
		
		put_scen_info();
		
		if(scen_headers.empty()) {
			me["next"].hide();
			me["prev"].hide();
		}
		
		me.run();
		return me.getResult<scen_header_type>();
	}
};

scen_header_type pick_a_scen() {
	return cChooseScenario().run();
}
