
#include <cstring>

#include "boe.global.hpp"

#include "universe.hpp"

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
#include "sounds.hpp"
#include <cstdio>
#include "boe.newgraph.hpp"
#include "boe.infodlg.hpp"
#include "utility.hpp"
#include "mathutil.hpp"
#include "strdlog.hpp"
#include "choicedlog.hpp"
#include "winutil.hpp"
#include "fileio.hpp"
#include "res_strings.hpp"
#include "scrollbar.hpp"
#include "button.hpp"
#include "pict.hpp"
#include <boost/lexical_cast.hpp>
#include "prefs.hpp"
#include "shop.hpp"
#include "cursors.hpp"

extern short stat_window;
extern eGameMode overall_mode;
extern bool changed_display_mode;
extern sf::RenderWindow mainPtr;
extern location ul;
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

extern std::vector<scen_header_type> scen_headers;

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
rectangle talk_area_rect = {5,5,420,284}, word_place_rect = {44,7,372,257},talk_help_rect = {5,254,21,272};
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
extern rectangle shopping_rects[8][7];
rectangle bottom_help_rects[4] = {{356,6,368,250},{374,6,386,270},{386,6,398,250},{398,6,410,250}};
rectangle shop_name_str = {44,6,56,200};
rectangle shop_frame = {62,10,352,269};
rectangle shop_done_rect = {388,212,411,275};
short shop_array[30];

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
			if(which_item < 0 || which_item >= 30) continue;
			cShopItem entry = active_shop.getItem(which_item);
			if(entry.quantity == 0) continue; // Just in case a stray entry accidentally gets put in for an infinite stock item
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
		for(int i = 0; i < 30; i++) {
			cShopItem item = active_shop.getItem(i);
			if(item.type != eShopItemType::EMPTY && univ.party.magic_store_items[active_shop_num][i].variety != eItemType::NO_ITEM)
				univ.party.magic_store_items[active_shop_num][i] = item.item;
			else univ.party.magic_store_items[active_shop_num][i].variety = eItemType::NO_ITEM;
		}
	}
	// We also need to save any limited stock
	for(int i = 0; i < 30; i++) {
		cShopItem item = active_shop.getItem(i);
		if(item.quantity > 0) {
			// This means the stock is limited.
			int left = item.type == eShopItemType::EMPTY ? 0 : item.quantity;
			univ.party.store_limited_stock[active_shop_num][i] = left;
		}
	}
}

void handle_shop_event(location p) {
	unsigned long store_what_picked;
	
	if(p.in(talk_help_rect)) {
		location loc = {p.x + ul.x, p.y + ul.y};
		if(!help_btn->handleClick(loc))
			return;
		give_help(226,27);
		return;
	}
	
	if(p.in(shop_done_rect)) {
		location loc = {p.x + ul.x, p.y + ul.y};
		if(done_btn->handleClick(loc))
			end_shop_mode();
		return;
	}
	
	p.x -= 5;
	p.y -= 5;
	
	for(short i = 0; i < 8; i++) {
		store_what_picked = shop_array[i + shop_sbar->getPosition()];
		if(store_what_picked >= 30) break;
		if(active_shop.getItem(store_what_picked).type == eShopItemType::EMPTY)
			break;
		if(p.in(shopping_rects[i][SHOPRECT_ACTIVE_AREA])) {
			click_shop_rect(shopping_rects[i][SHOPRECT_ACTIVE_AREA]);
			handle_sale(active_shop.getItem(store_what_picked), store_what_picked);
			set_up_shop_array();
			draw_shop_graphics(false, {});
		} else if(p.in(shopping_rects[i][SHOPRECT_ITEM_HELP])){
			click_shop_rect(shopping_rects[i][SHOPRECT_ITEM_HELP]);
			handle_info_request(active_shop.getItem(store_what_picked));
		}
	}
}

void handle_sale(cShopItem item, int i) {
	cItem base_item = item.item;
	short cost = item.getCost(active_shop.getCostAdjust());
	rectangle dummy_rect = {0,0,0,0};
	size_t size_before = active_shop.size();
	
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
				short s1, s2, s3;
				run_special(eSpecCtx::SHOPPING, 0, base_item.item_level, {0,0}, &s1, &s2, &s3);
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
				if(active_shop.size() != size_before)
					shop_sbar->setMaximum(shop_sbar->getMaximum() - 1);
				univ.current_pc().skills[skill]++;
			}
			break;
	}
	
	if(overall_mode != MODE_SHOPPING) {
		beep();
		ASB("Shop error 1. Report This!");
	}
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
	#ifdef _MSC_VER
		#pragma warning(push)
		#pragma warning(disable:4258)
	#endif
	int i = 0;
	for(int j = 0; i < 30 && j < 30; j++) {
		cShopItem entry = active_shop.getItem(j);
		switch(entry.type) {
			case eShopItemType::ITEM:
			case eShopItemType::MAGE_SPELL:
			case eShopItemType::PRIEST_SPELL:
			case eShopItemType::ALCHEMY:
			case eShopItemType::SKILL:
				shop_array[i++] = j;
				break;
			case eShopItemType::HEAL_WOUNDS:
				if(univ.current_pc().cur_health < univ.current_pc().max_health)
					shop_array[i++] = j;
				break;
			case eShopItemType::CURE_POISON:
				if(univ.current_pc().status[eStatus::POISON] > 0)
					shop_array[i++] = j;
				break;
			case eShopItemType::CURE_DISEASE:
				if(univ.current_pc().status[eStatus::DISEASE] > 0)
					shop_array[i++] = j;
				break;
			case eShopItemType::CURE_ACID:
				if(univ.current_pc().status[eStatus::ACID] > 0)
					shop_array[i++] = j;
				break;
			case eShopItemType::CURE_PARALYSIS:
				if(univ.current_pc().status[eStatus::PARALYZED] > 0)
					shop_array[i++] = j;
				break;
			case eShopItemType::CURE_DUMBFOUNDING:
				if(univ.current_pc().status[eStatus::DUMB] > 0)
					shop_array[i++] = j;
				break;
			case eShopItemType::DESTONE:
				if(univ.current_pc().main_status == eMainStatus::STONE)
					shop_array[i++] = j;
				break;
			case eShopItemType::RAISE_DEAD:
				if(univ.current_pc().main_status == eMainStatus::DEAD)
					shop_array[i++] = j;
				break;
			case eShopItemType::RESURRECT:
				if(univ.current_pc().main_status == eMainStatus::DUST)
					shop_array[i++] = j;
				break;
			case eShopItemType::REMOVE_CURSE:
				for(int i = 0; i < univ.current_pc().items.size(); i++) {
					if((univ.current_pc().equip[i]) && (univ.current_pc().items[i].cursed)) {
						shop_array[i++] = j;
						break;
					}
				}
				break;
			case eShopItemType::CALL_SPECIAL:
				if(PSD[entry.item.abil_data[0]][entry.item.abil_data[1]])
					shop_array[i++] = j;
				break;
			case eShopItemType::OPT_ITEM:
				entry.quantity %= 1000;
			case eShopItemType::TREASURE:
			case eShopItemType::CLASS:
				entry.type = eShopItemType::ITEM;
				entry.item = univ.party.magic_store_items[active_shop_num][j];
				if(entry.item.variety == eItemType::NO_ITEM)
					entry.type = eShopItemType::EMPTY;
				else shop_array[i++] = j;
				entry.quantity = 1;
				active_shop.replaceItem(j, entry);
				break;
			case eShopItemType::EMPTY:
				break;
		}
	}
	#ifdef _MSC_VER
		#pragma warning(pop)
	#endif
	shop_sbar->setMaximum(i - 8);
	std::fill(shop_array + i, shop_array + 30, -1);
}

void start_talk_mode(short m_num,short personality,mon_num_t monst_type,short store_face_pic) {
	rectangle area_rect;
	std::string place_string1;
	
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
	stat_screen_mode = MODE_SHOP;
	current_talk_node = TALK_LOOK;
	
	// Bring up and place first strings.
	place_string1 = univ.town.cur_talk().people[personality % 10].look;
	can_save_talk = true;
	
	place_talk_str(place_string1, "", 0, dummy_rect);
	
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
				if(quest.flags % 10 == 1)
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
	cDialog job_dlg("job-bank");
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
	short get_pc,s1 = -1,s2 = -1,s3 = -1;
	char asked[4];
	
	short a,b,c,d;
	eTalkNode ttype;
	
	if(p.in(talk_help_rect)) {
		location loc = {p.x + ul.x, p.y + ul.y};
		if(!help_btn->handleClick(loc))
			return;
		give_help(205,6);
		return;
	}
	
	p.x -= 5;
	p.y -= 5;
	
	int which_talk_entry = TALK_DUNNO;
	for(word_rect_t& word : talk_words) {
		if(word.node == -1) continue;
		if(!p.in(word.rect)) continue;
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
			if(univ.party.save_talk(univ.town->talking.people[store_personality].title, univ.town->name, save_talk_str1, save_talk_str2)) {
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
				do_rest(700, 30 & b, 25 * b);
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
			run_special(eSpecCtx::TALK,2,a,univ.party.town_loc,&s1,&s2,&s3);
			// check s1 & s2 to see if we got diff str, and, if so, munch old strs
			if((s1 >= 0) || (s2 >= 0)) {
				save_talk_str1 = s1 >= 0 ? univ.town->spec_strs[s1] : "";
				save_talk_str2 = s2 >= 0 ? univ.town->spec_strs[s2] : "";
			}
			get_strs(save_talk_str1,save_talk_str2,2,s1,s2);
			put_pc_screen();
			put_item_screen(stat_window);
			break;
		case eTalkNode::CALL_SCEN_SPEC:
			run_special(eSpecCtx::TALK,0,a,univ.party.town_loc,&s1,&s2,&s3);
			// check s1 & s2 to see if we got diff str, and, if so, munch old strs
			if((s1 >= 0) || (s2 >= 0)) {
				save_talk_str1 = s1 >= 0 ? univ.scenario.spec_strs[s1] : "";
				save_talk_str2 = s2 >= 0 ? univ.scenario.spec_strs[s2] : "";
			}
			get_strs(save_talk_str1,save_talk_str2,0,s1,s2);
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
	}
	save_prefs(reset_help);
	return true;
}

void pick_preferences() {
	set_cursor(sword_curs);
	
	cDialog prefsDlog("preferences");
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
	
	void (*give_help)(short,short,cDialog&) = ::give_help;
	
	int store_display_mode = get_int_pref("DisplayMode");
	prefsDlog.run(std::bind(give_help, 55, 0, std::ref(prefsDlog)));
	
	if(get_int_pref("DisplayMode") != store_display_mode)
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
	
	cDialog pcDialog("edit-party");
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
		if(page == ResMgr::get<StringRsrc>("tips")->size() - 50)
			page = 0;
		place_str = get_str("tips",50 + page);
		me["tip"].setText(place_str);
	}
	return true;
}

void tip_of_day() {
	using namespace std::placeholders;
	
	short page = get_ran(1,0,ResMgr::get<StringRsrc>("tips")->size() - 51);
	
	set_cursor(sword_curs);
	
	cDialog tips("tip-of-day");
	tips.attachClickHandlers(std::bind(tip_of_day_event_filter, _1, _2, std::ref(page)),{"done","next"});
	
	tips["tip"].setText(get_str("tips",50 + page));
	
	cLed& showAtStart = dynamic_cast<cLed&>(tips["onstart"]);
	showAtStart.setState(get_bool_pref("GiveIntroHint", true) ? led_red : led_off);
	
	tips.run();
	set_pref("GiveIntroHint", showAtStart.getState() != led_off);
	save_prefs();
	
}

static void put_scen_info(cDialog& me, short page) {
	std::ostringstream sout;
	static const char *difficulty[] = {"Low","Medium","High","Very High"};
	
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
		}
		else {
			me["pic" + n].hide();
			me["desc" + n].setText("");
			me["start" + n].hide();
		}
	}
}

static bool pick_a_scen_event_filter(cDialog& me, std::string item_hit, short& page) {
	if(item_hit == "cancel") {
		me.setResult<short>(-1);
		me.toast(false);
	} else if(item_hit == "next") {
		if(page == 0)
			page = (scen_headers.size() - 1) / 3;
		else page--;
		put_scen_info(me, page);
	} else if(item_hit == "prev") {
		if(page == (scen_headers.size() - 1) / 3)
			page = 0;
		else page++;
		put_scen_info(me, page);
	} else if(item_hit.substr(0,item_hit.length()-1) == "start") {
		int scen_hit = item_hit[item_hit.length()-1] - '1';
		me.setResult<short>(scen_hit + page * 3);
		me.toast(true);
	}
	return true;
}

short pick_a_scen() {
	using namespace std::placeholders;
	extern fs::path scenDir;
	// TODO: Add a button to jump to the scenarios folder
	build_scen_headers();
	
	if(scen_headers.empty()) {
		cChoiceDlog err("no-scenarios");
		err->getControl("path").setText(scenDir.string());
		err.show();
		return -1;
	}
	set_cursor(sword_curs);
	
	cDialog pickScen("pick-scenario");
	short page = 0;
	pickScen.attachClickHandlers(std::bind(pick_a_scen_event_filter, _1, _2, std::ref(page)), {"cancel", "next", "prev", "start1", "start2", "start3"});
	
	put_scen_info(pickScen, 0);
	
	if(scen_headers.size() <= 3) {
		pickScen["next"].hide();
		pickScen["prev"].hide();
	}
	
	pickScen.run();
	return pickScen.getResult<short>();
}

short pick_prefab_scen() {
	
	set_cursor(sword_curs);
	
	cChoiceDlog pickScenario("pick-prefab-scen", {"cancel", "scen1", "scen2", "scen3"});
	
	std::string item_hit = pickScenario.show();
	if(item_hit == "cancel") return -1;
	else return item_hit[4] - '1';
}
