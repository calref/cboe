
#include <cstdio>
#include <cstring>

#include "boe.global.hpp"

#include "universe/universe.hpp"

#include "boe.graphics.hpp"
#include "boe.text.hpp"
#include "boe.items.hpp"
#include "boe.specials.hpp"
#include "boe.party.hpp"
#include "boe.locutils.hpp"
#include "boe.newgraph.hpp"
#include "boe.infodlg.hpp"
#include "sounds.hpp"
#include "boe.monster.hpp"
#include "boe.main.hpp"
#include "mathutil.hpp"
#include "utility.hpp"
#include "dialogxml/dialogs/strdlog.hpp"
#include "dialogxml/dialogs/strchoice.hpp"
#include "dialogxml/dialogs/3choice.hpp"
#include "dialogxml/widgets/message.hpp"
#include <array>
#include <boost/lexical_cast.hpp>
#include "tools/prefs.hpp"
#include "tools/winutil.hpp"
#include "tools/cursors.hpp"
#include "fileio/resmgr/res_dialog.hpp"
#include "gfx/render_shapes.hpp"

extern short which_combat_type;
extern eGameMode overall_mode;
extern eItemWinMode stat_window;
extern bool boom_anim_active;
extern rectangle d_rects[80];
extern short d_rect_index[80];

extern bool map_visible;
extern cUniverse univ;

extern void draw_map(bool need_refresh);

short selected;

const int ITEMS_IN_WINDOW = 8;

bool GTP(short item_num) {
	cItem item = univ.scenario.get_stored_item(item_num);
	return univ.party.give_item(item,true);
}
bool silent_GTP(short item_num) {
	cItem item = univ.scenario.get_stored_item(item_num);
	return univ.party.give_item(item,false);
}

void give_gold(short amount,bool print_result) {
	if(amount < 0) return;
	univ.party.gold += amount;
	if(print_result) put_pc_screen();
}

bool take_gold(short amount,bool print_result) {
	if(univ.party.gold < amount)
		return false;
	univ.party.gold -= amount;
	if(print_result) put_pc_screen();
	return true;
}

void give_food(short amount,bool print_result) {
	if(amount < 0) return;
	univ.party.food = univ.party.food + amount;
	if(print_result) put_pc_screen();
}

short take_food(short amount,bool print_result) {
	short shortfall = amount - univ.party.food;
	if(shortfall > 0) {
		univ.party.food = 0;
	}else{
		univ.party.food -= amount;
		shortfall = 0;
	}
	
	if(print_result) put_pc_screen();
	return shortfall;
}

void equip_item(short pc_num,short item_num) {
	if(overall_mode == MODE_COMBAT && univ.party[pc_num].items[item_num].variety == eItemType::FOOD)
		add_string_to_buf("Equip: Not in combat");
	else {
		if(univ.party[pc_num].equip[item_num])
			univ.party[pc_num].unequip_item(item_num, true);
		else if(is_combat() && univ.party[pc_num].items[item_num].variety == eItemType::ARMOR)
			add_string_to_buf("Equip: Not armor in combat");
		else univ.party[pc_num].equip_item(item_num, true);
	}
	if(stat_window == pc_num)
		put_item_screen(stat_window);
}


void drop_item(short pc_num,short item_num,location where_drop) {
	short s1, s2;
	int spec = -1;
	std::string choice;
	short how_many = 1;
	cItem item_store;
	bool take_dropped_item = true, need_redraw = false;
	location loc;
	
	item_store = univ.party[pc_num].items[item_num];
	if(item_store.ability == eItemAbil::DROP_CALL_SPECIAL)
		spec = item_store.abil_strength;
	
	if(univ.party[pc_num].equip[item_num] && univ.party[pc_num].items[item_num].cursed)
		add_string_to_buf("Drop: Item is cursed.");
	else switch(overall_mode) {
		case MODE_OUTDOORS:
			choice = cChoiceDlog("drop-item-confirm",{"okay","cancel"}).show();
			if(choice == "cancel")
				return;
			add_string_to_buf("Drop: OK");
			if((item_store.type_flag > 0) && (item_store.charges > 1)) {
				how_many = get_num_of_items(item_store.charges);
				if(how_many == item_store.charges)
					univ.party[pc_num].take_item(item_num);
				else univ.party[pc_num].items[item_num].charges -= how_many;
			}
			else univ.party[pc_num].take_item(item_num);
			break;
			
		case MODE_DROP_TOWN: case MODE_DROP_COMBAT:
			loc = where_drop;
			if((item_store.type_flag > 0) && (item_store.charges > 1)) {
				how_many = get_num_of_items(item_store.charges);
				if(how_many <= 0)
					return;
				if(how_many < item_store.charges)
					take_dropped_item = false;
				item_store.charges = how_many;
			}
			if(place_item(item_store,loc,true)) {
				add_string_to_buf("Drop: Item put away");
				spec = -1; // Don't call drop specials if it was put away
			} else add_string_to_buf("Drop: OK");
			univ.party[pc_num].items[item_num].charges -= how_many;
			if(take_dropped_item)
				univ.party[pc_num].take_item(item_num);
			break;
		default: //should never be reached
			break;
	}
	if(spec >= 0)
		while(how_many--)
			run_special(eSpecCtx::DROP_ITEM, eSpecCtxType::SCEN, spec, where_drop, &s1, &s2, &need_redraw);
	if(need_redraw)
		draw_terrain(0);
}

bool place_item(cItem item,location where,bool contained) {
	if(contained && !is_container(where))
		contained = false;
	
	bool is_pushable_ctr = contained && (univ.town.is_barrel(where.x,where.y) || univ.town.is_crate(where.x,where.y));
	
	for(short i = 0; i < univ.town.items.size(); i++)
		if(univ.town.items[i].variety == eItemType::NO_ITEM) {
			univ.town.items[i] = item;
			univ.town.items[i].item_loc = where;
			univ.town.items[i].contained = contained;
			univ.town.items[i].held = is_pushable_ctr;
			reset_item_max();
			return univ.town.items[i].contained;
		}
	univ.town.items.push_back(item);
	univ.town.items.back().item_loc = where;
	univ.town.items.back().contained = contained;
	univ.town.items.back().held = is_pushable_ctr;
	reset_item_max();
	return univ.town.items.back().contained;
}

cItem item_store;

void give_thing(short pc_num, short item_num) {
	short who_to,how_many = 0;
	bool take_given_item = true;
	
	if(univ.party[pc_num].equip[item_num] && univ.party[pc_num].items[item_num].cursed)
		add_string_to_buf("Give: Item is cursed.");
	else {
		item_store = univ.party[pc_num].items[item_num];
		who_to = select_pc(eSelectPC::ONLY_CAN_GIVE_FROM_ACTIVE,"Give item to who?");
		// No party members can receive the item:
		if(who_to == 8){
			if(overall_mode == MODE_COMBAT){
				ASB("Can't give: must be adjacent with enough carrying capacity.", 2);
			}else{
				ASB("Can't give: no one has the carrying capacity.", 2);
			}
		}
		
		if((who_to < 6) && (who_to != pc_num)) {
			if((item_store.type_flag > 0) && (item_store.charges > 1)) {
				how_many = get_num_of_items(item_store.charges);
				if(how_many == 0)
					return;
				if(how_many < item_store.charges)
					take_given_item = false;
				univ.party[pc_num].items[item_num].charges -= how_many;
				item_store.charges = how_many;
			}
			eBuyStatus give_status = univ.party[who_to].give_item(item_store,0);
			if(give_status != eBuyStatus::OK){
				// This should be impossible, because select_pc() already checked that the options
				// were viable.
				showFatalError("Unexpectedly failed to give item!");
			}
			if(take_given_item){
				univ.party[pc_num].take_item(item_num);
			}
		}
	}
}

// Procedure only ready for town and outdoor
short dist_from_party(location where) {
	short store = 1000;
	
	if(is_combat()) {
		for(short i = 0; i < 6; i++)
			if(univ.party[i].main_status == eMainStatus::ALIVE)
				store = min(store,dist(univ.party[i].combat_pos,where));
	}
	else store = dist(univ.party.town_loc,where);
	
	return store;
}

// This marks the item as taken so that it won't show up again when the party returns to this town
void set_item_flag(cItem* item) {
	if((item->is_special > 0) && (item->is_special < 65)) {
		item->is_special--;
		univ.town->set_item_taken(item->is_special);
		item->is_special = 0;
	}
}

//short pc_num; // if 6, any
short get_item(location place,short pc_num,bool check_container) {
	short taken = 0;
	bool item_near = false;
	short mass_get = 1;
	
	for(short i = 0; i < univ.town.monst.size(); i++)
		if(univ.town.monst[i].is_alive() && !univ.town.monst[i].is_friendly()
			&& (can_see_light(place,univ.town.monst[i].cur_loc,sight_obscurity) < 5))
			mass_get = 0;
	
	for(short i = 0; i < univ.town.items.size(); i++)
		if(univ.town.items[i].variety != eItemType::NO_ITEM)
			if(((adjacent(place,univ.town.items[i].item_loc)) ||
				 (mass_get == 1 && !check_container &&
				  ((dist(place,univ.town.items[i].item_loc) <= 4) || ((is_combat()) && (which_combat_type == 0)))
				  && (can_see_light(place,univ.town.items[i].item_loc,sight_obscurity) < 5)))
				&& ((!univ.town.items[i].contained) || (check_container))) {
				taken = 1;
				
				if(univ.town.items[i].value < 2)
					univ.town.items[i].ident = true;
				item_near = true;
			}
	if(item_near)
		if(display_item(place,pc_num,mass_get,check_container)) { // if true, there was a theft
			for(short i = 0; i < univ.town.monst.size(); i++)
				if(univ.town.monst[i].is_alive() && univ.town.monst[i].is_friendly()
					&& (can_see_light(place,univ.town.monst[i].cur_loc,sight_obscurity) < 5)) {
					make_town_hostile();
					add_string_to_buf("Your crime was seen!");
					break;
				}
		}
	
	if(pc_num != 10) {
		if(taken == 0)
			add_string_to_buf("Get: nothing here");
		else add_string_to_buf("Get: OK");
	}
	
	reset_item_max();
	
	return taken;
	
}

void make_town_hostile() {
	set_town_attitude(0, -1, eAttitude::HOSTILE_A);
	return;
}

// Set Attitude node adapted from *i, meant to replace make_town_hostile node
void set_town_attitude(short lo,short hi,eAttitude att) {
	short num;
	
	if(is_combat() && which_combat_type == 0)
		return;

	give_help(53,0);

	static std::set<eAttitude> hostile_attitudes = { eAttitude::HOSTILE_A, eAttitude::HOSTILE_B };
	univ.town.monst.hostile = hostile_attitudes.count(att);

	long long num_monst = univ.town.monst.size();
	
	// Nice smart indexing, like Python :D
	if(lo <= -num_monst)
		lo = 0;
	if(lo < 0)
		lo = num_monst + lo;
	if(hi <= -num_monst)
		hi = 0;
	if(hi < 0)
		hi = num_monst + hi;
	if(hi < lo)
		std::swap(lo, hi);
	
	for(short i = lo; i <= hi; i++) {
		if(univ.town.monst[i].is_alive() && univ.town.monst[i].summon_time == 0){
			univ.town.monst[i].attitude = att;
			num = univ.town.monst[i].number;
			// If made hostile, make mobile
			if(!univ.town.monst[i].is_friendly()) {
				univ.town.monst[i].mobility = 1;
				// If a "guard", give a power boost
				if(univ.scenario.scen_monsters[num].guard) {
					univ.town.monst[i].active = eCreatureStatus::ALERTED;
					univ.town.monst[i].health *= 3;
					univ.town.monst[i].status[eStatus::HASTE_SLOW] = 8;
					univ.town.monst[i].status[eStatus::BLESS_CURSE] = 8;
				}
				
			}
		}
	}
	
	// In some towns, doin' this'll getcha' killed.
	// (Or something else! Killing the party would be the responsibility of whatever special node is called.)
	if(univ.town.monst.hostile && univ.town->spec_on_hostile >= 0)
		run_special(eSpecCtx::TOWN_HOSTILE, eSpecCtxType::TOWN, univ.town->spec_on_hostile, univ.party.town_loc);
}

// TODO: Set town status to "dead"? Could reuse above with magic att (eg -1), or write new function.


static void put_item_graphics(cDialog& me, size_t& first_item_shown, short& current_getting_pc, const std::vector<cItem*>& item_array) {
	cItem item;
	char key_stash[2] = "a";
	
	// First make sure all arrays for who can get stuff are in order.
	if(current_getting_pc < 6 && (univ.party[current_getting_pc].main_status != eMainStatus::ALIVE
			|| !univ.party[current_getting_pc].has_space())) {
	 	current_getting_pc = 6;
	 	
	}
	
	for(short i = 0; i < 6; i++) {
		std::ostringstream sout;
		sout << "pc" << i + 1;
		std::string id = sout.str();
		if(univ.party[i].main_status == eMainStatus::ALIVE && univ.party[i].has_space()
		   && ((!is_combat()) || (univ.cur_pc == i))) {
			if(current_getting_pc == 6)
				current_getting_pc = i;
			me[id].show();
		} else {
			me[id].hide();
			sout << "-g";
			me[sout.str()].hide();
		}
		if(current_getting_pc == i)
			me.addLabelFor(id, "*   ", LABEL_LEFT, 7, true);
		else me.addLabelFor(id,"    ", LABEL_LEFT, 7, true);
	}
	
	// darken arrows, as appropriate
	if(first_item_shown == 0)
		me["up"].hide();
	else me["up"].show();
	if(item_array.size() <= ITEMS_IN_WINDOW || first_item_shown > item_array.size() - (ITEMS_IN_WINDOW-1))
		me["down"].hide();
	else me["down"].show();
	
	for(short i = 0; i < ITEMS_IN_WINDOW; i++) {
		std::ostringstream sout;
		sout << "item" << i + 1;
		std::string pict = sout.str() + "-g", name = sout.str() + "-name";
		std::string detail = sout.str() + "-detail", weight = sout.str() + "-weight";
		std::string key = sout.str() + "-key";
		key_stash[0] = 'a' + i;
		
		// TODO: Rework this so that no exceptions are required
		if(i + first_item_shown < item_array.size() && item_array[i + first_item_shown]->variety != eItemType::NO_ITEM) {
			// display an item in window
			me[pict].show();
			item = *item_array[i + first_item_shown];
			me[name].setText(item.ident ? item.full_name : item.name);
			// TODO: Party sheet items
			cPict& pic = dynamic_cast<cPict&>(me[pict]);
			if(item.graphic_num >= 1000)
				pic.setPict(item.graphic_num - 1000, PIC_CUSTOM_ITEM);
			else pic.setPict(item.graphic_num, PIC_ITEM);
			me[detail].setText(item.interesting_string());
			me[weight].setText("Weight: " + std::to_string(item.item_weight()));
			me[key].setText(key_stash);
		} else { // erase the spot
			me[pict].hide();
			me[name].setText("");
			me[detail].setText("");
			me[weight].setText("");
			me[key].setText("");
		}
	}
	
	if(current_getting_pc < 6) {
		std::ostringstream sout;
		sout << univ.party[current_getting_pc].name << " is carrying ";
		sout << univ.party[current_getting_pc].cur_weight() << " out of " << univ.party[current_getting_pc].max_weight() << '.';
		me["prompt"].setText(sout.str());
	}
	
	for(short i = 0; i < 6; i++)
		if(univ.party[i].main_status == eMainStatus::ALIVE) {
			std::ostringstream sout;
			sout << "pc" << i + 1 << "-g";
			pic_num_t pic = univ.party[i].which_graphic;
			if(pic >= 100 && pic < 1000)
				dynamic_cast<cPict&>(me[sout.str()]).setPict(pic - 100,PIC_MONST);
			else dynamic_cast<cPict&>(me[sout.str()]).setPict(pic,PIC_PC);
		}
}


static bool display_item_event_filter(cDialog& me, std::string id, size_t& first_item_shown, short& current_getting_pc, std::vector<cItem*>& item_array, bool allow_overload) {
	cItem item;
	
	if(id == "done") {
		me.toast(true);
	} else if(id == "up") {
		if(first_item_shown > 0) {
			first_item_shown -= ITEMS_IN_WINDOW;
			put_item_graphics(me, first_item_shown, current_getting_pc, item_array);
		}
	} else if(id ==  "down") {
		if(first_item_shown + ITEMS_IN_WINDOW < item_array.size()) {
			first_item_shown += ITEMS_IN_WINDOW;
			put_item_graphics(me, first_item_shown, current_getting_pc, item_array);
		}
	} else if(id.substr(0,2) == "pc") {
		current_getting_pc = id[2] - '1';
		put_item_graphics(me, first_item_shown, current_getting_pc, item_array);
	} else {
		if(current_getting_pc == 6) return true;
		size_t item_hit;
		item_hit = id[4] - '1';
		item_hit += first_item_shown;
		if(item_hit >= item_array.size()) return true;
		item = *item_array[item_hit];
		if(item.property) {
			if(!me.getResult<bool>()) {
				std::string choice = cChoiceDlog("steal-item",{"steal","leave"}, &me).show();
				if(choice == "leave") return true;
				me.setResult(true);
			}
			item.property = false;
		}
		
		if(item.variety == eItemType::GOLD) {
			if(item.item_level > 3000)
				item.item_level = 3000;
			set_item_flag(&item);
			give_gold(item.item_level,false);
			play_sound(39); // formerly force_play_sound
		} else if(item.variety == eItemType::FOOD) {
			give_food(item.item_level,false);
			set_item_flag(&item);
			play_sound(62); // formerly force_play_sound
		} else if(item.variety == eItemType::SPECIAL) {
			univ.party.spec_items.insert(item.item_level);
			set_item_flag(&item);
		} else if(item.variety == eItemType::QUEST) {
			univ.party.active_quests[item.item_level].status = eQuestStatus::STARTED;
			univ.party.active_quests[item.item_level].start = univ.party.calc_day();
			univ.party.active_quests[item.item_level].source = -1;
			set_item_flag(&item);
		} else {
			if(!allow_overload && item.item_weight() > univ.party[current_getting_pc].free_weight()) {
				play_sound(41);
				me["prompt"].setText("It's too heavy to carry.");
				give_help(38,0,me);
				return true;
			}
			
			set_item_flag(&item);
			play_sound(0); // formerly force_play_sound
			int flags = GIVE_DO_PRINT;
			if(allow_overload)
				flags |= GIVE_ALLOW_OVERLOAD;
			univ.party[current_getting_pc].give_item(item, flags);
		}
		*item_array[item_hit] = cItem();
		item_array.erase(item_array.begin() + item_hit);
		put_item_graphics(me, first_item_shown, current_getting_pc, item_array);
	}
	return true;
}

// Returns true if a theft committed
//pc_num;  // < 6 - this pc only  6 - any pc
//short mode; // 0 - adjacent  1 - all in sight
bool display_item(location from_loc,short /*pc_num*/,short mode, bool check_container) {
//	short item_array[130];
	std::vector<cItem*> item_array;
	
	set_cursor(sword_curs);
	
	short current_getting_pc = univ.cur_pc;
	
	for(short i = 0; i < univ.town.items.size(); i++)
		if(univ.town.items[i].variety != eItemType::NO_ITEM) {
			if(((adjacent(from_loc,univ.town.items[i].item_loc)) ||
				 (mode == 1 && !check_container &&
				  ((dist(from_loc,univ.town.items[i].item_loc) <= 4) || ((is_combat()) && (which_combat_type == 0)))
				  && (can_see_light(from_loc,univ.town.items[i].item_loc,sight_obscurity) < 5))) &&
				(univ.town.items[i].contained == check_container) &&
				((!check_container) || (univ.town.items[i].item_loc == from_loc))) {
				item_array.push_back(&univ.town.items[i]);
			}
		}
	
	bool stole_something = false;
	if(check_container)
		stole_something = show_get_items("Looking in container:", item_array, current_getting_pc);
	else if(mode == 0)
		stole_something = show_get_items("Getting all adjacent items:", item_array, current_getting_pc);
	else stole_something = show_get_items("Getting all nearby items:", item_array, current_getting_pc);
	
	put_item_screen(stat_window);
	put_pc_screen();
	
	return stole_something;
}

bool show_get_items(std::string titleText, std::vector<cItem*>& itemRefs, short pc_getting, bool overload) {
	using namespace std::placeholders;
	size_t first_item = 0;
	
	cDialog itemDialog(*ResMgr::dialogs.get("get-items"));
	auto handler = std::bind(display_item_event_filter, _1, _2, std::ref(first_item), std::ref(pc_getting), std::ref(itemRefs), overload);
	itemDialog.attachClickHandlers(handler, {"done", "up", "down"});
	itemDialog.attachClickHandlers(handler, {"pc1", "pc2", "pc3", "pc4", "pc5", "pc6"});
	itemDialog.setResult(false);
	cTextMsg& title = dynamic_cast<cTextMsg&>(itemDialog["title"]);
	
	title.setText(titleText);
	
	for(int i = 1; i <= ITEMS_IN_WINDOW; i++) {
		std::ostringstream sout;
		sout << "item" << i << "-key";
		itemDialog[sout.str()].attachKey({false, static_cast<unsigned char>('`' + i), mod_none});
		itemDialog[sout.str()].attachClickHandler(handler);
	}
	put_item_graphics(itemDialog, first_item, pc_getting, itemRefs);
		
	itemDialog.setResult(false); // The result is set to true if the player steals something
	itemDialog.runWithHelp(36, 37);
	
	return itemDialog.getResult<bool>();
	
	
}

void custom_pic_dialog(std::string title, pic_num_t bigpic) {
	cDialog pic_dlg(*ResMgr::dialogs.get("show-map"));
	cControl& okay = pic_dlg["okay"];
	cControl& text = pic_dlg["title"];
	okay.attachClickHandler(std::bind(&cDialog::toast, &pic_dlg, false));
	text.setText(title);
	cPict& map = dynamic_cast<cPict&>(pic_dlg["map"]);
	// We don't provide a way to use non-custom full sheets - why would you want to show standard help graphics?
	map.setPict(bigpic, PIC_CUSTOM_FULL);
	// Now we need to adjust the size to ensure that everything fits correctly.
	map.recalcRect();
	rectangle mapBounds = map.getBounds();
	rectangle btnBounds = okay.getBounds();
	rectangle txtBounds = text.getBounds();
	btnBounds.offset(-btnBounds.left, -btnBounds.top);
	btnBounds.offset(mapBounds.right - btnBounds.width(), mapBounds.bottom + 10);
	okay.setBounds(btnBounds);
	txtBounds.right = mapBounds.right;
	text.setBounds(txtBounds);
	pic_dlg.recalcRect();
	pic_dlg.run();
}

void story_dialog(std::string title, str_num_t first, str_num_t last, eSpecCtxType which_str_type, pic_num_t pic, ePicType pt, short anim_loops, int anim_fps) {
	cDialog story_dlg(*ResMgr::dialogs.get("many-str"));
	dynamic_cast<cPict&>(story_dlg["pict"]).setPict(pic, pt);
	setup_dialog_pict_anim(story_dlg, "pict", anim_loops, anim_fps);
	str_num_t cur = first;
	story_dlg.attachClickHandlers([&cur,first,last,which_str_type](cDialog& me, std::string clicked, eKeyMod) -> bool {
		if(clicked == "left") {
			if(cur > first) cur--;
		} else if(clicked == "done" || cur == last) {
			me.toast(false);
			return true;
		} else if(clicked == "right") {
			cur++;
		}
		std::string text;
		univ.get_str(text, which_str_type, cur);
		me["str"].setText(text);
		return true;
	}, {"left", "right", "done"});
	story_dlg["left"].triggerClickHandler(story_dlg, "left", eKeyMod());
	story_dlg["title"].setText(title);
	story_dlg.run();
}

static bool get_num_of_items_event_filter(cDialog& me, std::string item_hit, eKeyMod) {
	if(item_hit == "cancel"){
		me.setResult<int>(0);
		me.toast(false);
	}
	else if(me.toast(true)){
		me.setResult<int>(me["number"].getTextAsNum());
	}
	return true;
}

//town_num; // Will be 0 - 200 for town, 200 - 290 for outdoors
//short sign_type; // terrain type
short get_num_of_items(short max_num) {
	set_cursor(sword_curs);
	
	cDialog numPanel(*ResMgr::dialogs.get("get-num"));
	numPanel["extra-led"].hide();
	numPanel.attachClickHandlers(get_num_of_items_event_filter, {"okay", "cancel"});
	
	numPanel["prompt"].setText("How many? (0-" + std::to_string(max_num) + ") ");
	numPanel["number"].setTextToNum(max_num);
	numPanel.run();
	
	return minmax(0,max_num,numPanel.getResult<int>());
}

void init_mini_map() {
	double map_scale = get_ui_scale_map();
	if (map_scale < 0.1) map_scale = 1.0;
	if (mini_map().isOpen()) mini_map().close();
	mini_map().create(sf::VideoMode(map_scale*296,map_scale*277), "Map", sf::Style::Titlebar | sf::Style::Close);
	// TODO why is 52,62 the default position, anyway?
	int map_x = get_int_pref("MapWindowX", 52);
	int map_y = get_int_pref("MapWindowY", 62);
	mini_map().setPosition(sf::Vector2i(map_x,map_y));
	sf::View view;
	view.reset(sf::FloatRect(0, 0, map_scale*296,map_scale*277));
	view.setViewport(sf::FloatRect(0, 0, map_scale, map_scale));
	mini_map().setView(view);
	mini_map().setVisible(false);
	map_visible=false;
	setWindowFloating(mini_map(), true);
	makeFrontWindow(mainPtr(), mini_map());
	
	// Create and initialize map gworld
	if(!map_gworld().create(384, 384)) {
		play_sound(2);
		throw std::string("Failed to initialized automap!");
	} else {
		draw_map(true);
	}
}

void place_glands(location where,mon_num_t m_type) {
	cItem store_i;
	cMonster monst;
	
	if(m_type >= 10000)
		monst = univ.party.summons[m_type - 10000];
	else monst = univ.scenario.scen_monsters[m_type];
	
	if((monst.corpse_item >= 0) && (monst.corpse_item < 400) && (get_ran(1,1,100) < monst.corpse_item_chance)) {
		store_i = univ.scenario.get_stored_item(monst.corpse_item);
		place_item(store_i,where);
	}
}

void reset_item_max() {
	while(!univ.town.items.empty() && univ.town.items.back().variety == eItemType::NO_ITEM)
		univ.town.items.pop_back();
}

short item_val(cItem item) {
	if(item.charges == 0)
		return item.value;
	return item.charges * item.value;
}

//// rewrite
//short mode;  // 0 - normal, 1 - force
void place_treasure(location where,short level,short loot,short mode) {
	
	cItem new_item;
	short amt,r1;
	// Make these static const because they are never changed.
	// Saves them being initialized every time the function is called.
	static const short treas_chart[5][6] = {
		{0,-1,-1,-1,-1,-1},
		{1,-1,-1,-1,-1,-1},
		{2,1,1,-1,-1,-1},
		{3,2,1,1,-1,-1},
		{4,3,2,2,1,1}
	};
	static const short treas_odds[5][6] = {
		{10,0,0,0,0,0},
		{50,0,0,0,0,0},
		{60,50,40,0,0,0},
		{100,90,80,70,0,0},
		{100,80,80,75,75,75}
	};
	static const short id_odds[21] = {
		0,10,15,20,25,30,35,
		39,43,47,51,55,59,63,
		67,71,73,75,77,79,81
	};
	static const short max_mult[5][10] = {
		{0,0,0,0,0,0,0,0,0,1},
		{0,0,1,1,1,1,2,3,5,20},
		{0,0,1,1,2,2,4,6,10,25},
		{5,10,10,10,15,20,40,80,100,100},
		{25,25,50,50,50,100,100,100,100,100}
	};
	static const short min_chart[5][10] = {
		{0,0,0,0,0,0,0,0,0,1},
		{0,0,0,0,0,0,0,0,5,20},
		{0,0,0,0,1,1,5,10,15,40},
		{10,10,15,20,20,30,40,50,75,100},
		{50,100,100,100,100,200,200,200,200,200}
	};
	short max,min;
	
	if(loot == 1)
		amt = get_ran(2,1,7) + 1;
	else amt = loot * (get_ran(1,0,10 + (loot * 6) + (level * 2)) + 5);
	
	if(univ.party.get_level() <= 12)
		amt += 1;
	if(univ.party.get_level() <= 60 && amt > 2)
		amt += 2;
	
	if(amt > 3) {
		new_item = univ.scenario.get_stored_item(0);
		new_item.item_level = amt;
		r1 = get_ran(1,1,9);
		if(((loot > 1) && (r1 < 7)) || ((loot == 1) && (r1 < 5)) || (mode == 1)
			|| ((r1 < 6) && (univ.party.get_level() < 30)) || (loot > 2) )
			place_item(new_item,where);
	}
	for(short j = 0; j < 5; j++) {
		r1 = get_ran(1,1,100);
		if((treas_chart[loot][j] >= 0) && (r1 <= treas_odds[loot][j] + check_party_stat(eSkill::LUCK, 0))) {
			r1 = get_ran(1,0,9);
			min = min_chart[treas_chart[loot][j]][r1];
			r1 = get_ran(1,0,9);
			max = (min + level + (2 * (loot - 1)) + (check_party_stat(eSkill::LUCK, 0) / 3)) * max_mult[treas_chart[loot][j]][r1];
			if(get_ran(1,0,1000) == 500) {
				max = 10000;
				min = 100;
			}
			
			// reality check
			if((loot == 1) && (max > 100) && (get_ran(1,0,8) < 7))
				max = 100;
			if((loot == 2) && (max > 200) && (get_ran(1,0,8) < 6))
				max = 200;
			
			
			new_item = univ.scenario.return_treasure(treas_chart[loot][j]);
			if((item_val(new_item) < min) || (item_val(new_item) > max)) {
				new_item = univ.scenario.return_treasure(treas_chart[loot][j]);
				if((item_val(new_item) < min) || (item_val(new_item) > max)) {
					new_item = univ.scenario.return_treasure(treas_chart[loot][j]);
					if(item_val(new_item) > max)
						new_item.variety = eItemType::NO_ITEM;
				}
			}
			
			// not many magic items
			if(mode == 0) {
				if(new_item.magic && (level < 2) && (get_ran(1,0,5) < 3))
					new_item.variety = eItemType::NO_ITEM;
				if(new_item.magic && (level == 2) && (get_ran(1,0,5) < 2))
					new_item.variety = eItemType::NO_ITEM;
				if(new_item.cursed && (get_ran(1,0,5) < 3))
					new_item.variety = eItemType::NO_ITEM;
			}
			
			// if forced, keep dipping until a treasure comes up
			if((mode == 1)	&& (max >= 20)) {
				do new_item = univ.scenario.return_treasure(treas_chart[loot][j]);
				while(new_item.variety == eItemType::NO_ITEM || item_val(new_item) > max);
			}
			
			// Not many cursed items
			if(new_item.cursed && (get_ran(1,0,2) == 1))
				new_item.variety = eItemType::NO_ITEM;
			
			if(new_item.variety != eItemType::NO_ITEM) {
				for(short i = 0; i < 6; i++)
					if((univ.party[i].main_status == eMainStatus::ALIVE)
					   && get_ran(1,1,100) < id_odds[univ.party[i].skill(eSkill::ITEM_LORE)])
						new_item.ident = true;
				place_item(new_item,where);
			}
		}
	}
}

static bool get_text_response_event_filter(cDialog& me, std::string item_hit, eKeyMod) {
	me.toast(true);
	if(item_hit == "cancel") me.setResult(std::string {""});
	else me.setResult(me["response"].getText());
	return true;
}

std::string get_text_response(std::string prompt, pic_num_t pic) {
	set_cursor(sword_curs);
	
	cDialog strPanel(*ResMgr::dialogs.get("get-response"));
	strPanel.attachClickHandlers(get_text_response_event_filter, {"okay", "cancel"});
	if(!prompt.empty()) {
		dynamic_cast<cPict&>(strPanel["pic"]).setPict(pic);
		strPanel["prompt"].setText(prompt);
	}
	
	strPanel.run();
	// Note: Originally it only changed the first 15 characters.
	std::string result = strPanel.getResult<std::string>();
	std::transform(result.begin(), result.end(), result.begin(), tolower);
	return result;
}

static bool select_pc_event_filter (cDialog& me, std::string item_hit, eKeyMod) {
	me.toast(true);
	if(item_hit == "pick-all"){
		me.setResult<short>(7);
	}else if(item_hit != "cancel"){
		short which_pc = item_hit[item_hit.length() - 1] - '1';
		me.setResult<short>(which_pc);
	}else me.setResult<short>(6);
	return true;
}

short select_pc(eSelectPC mode, std::string title, eSkill highlight_highest, bool allow_choose_all) {
	short item_hit;
	
	set_cursor(sword_curs);
	
	cDialog selectPc(*ResMgr::dialogs.get("select-pc"));
	selectPc.attachClickHandlers(select_pc_event_filter, {"cancel", "pick1", "pick2", "pick3", "pick4", "pick5", "pick6", "pick-all"});
	
	// The default title is defined in select-pc.xml
	if(!title.empty())
		selectPc["title"].setText(title);
	
	bool any_options = false;
	std::array<short, 6> pc_skills = {0, 0, 0, 0, 0, 0};
	short highest_skill = 0;
	short last_skill = 0;
	bool all_pcs_equal = true;
	for(short i = 0; i < 6; i++) {
		std::string n = boost::lexical_cast<std::string>(i + 1);
		bool can_pick = true;
		std::string extra_info = "";
		if(univ.party[i].main_status == eMainStatus::ABSENT || univ.party[i].main_status == eMainStatus::FLED)
			can_pick = false;

		if(mode != eSelectPC::ANY && univ.party[i].main_status >= eMainStatus::SPLIT)
			can_pick = false;

		else switch(mode) {
			case eSelectPC::ONLY_CAN_GIVE_FROM_ACTIVE:
				if(i == univ.cur_pc){
					can_pick = false;
					break;
				}
				if((overall_mode == MODE_COMBAT) && univ.party[i].is_alive() && !adjacent(univ.party[univ.cur_pc].combat_pos,univ.party[i].combat_pos)) {
					can_pick = false;
					extra_info = "too far away";
					break;
				}
				BOOST_FALLTHROUGH;
			case eSelectPC::ONLY_CAN_GIVE:
				switch(univ.party[i].can_give_item(item_store)){
					case eBuyStatus::TOO_HEAVY:
						extra_info = "item too heavy";
						if(false) // skip first line of fallthrough
					case eBuyStatus::NO_SPACE:
						extra_info = "no item slot";
						can_pick = false;
						break;
					case eBuyStatus::DEAD:
						// Extra info not really needed, and kind of silly to print
						extra_info = "";
						can_pick = false;
						break;
					default:
						break;
				}
				break;
			case eSelectPC::ONLY_LIVING_WITH_ITEM_SLOT:
				if(!univ.party[i].has_space()){
					can_pick = false;
					extra_info = "no item slot";
				}
				if(false) // Skip fallthrough past ONLY_CAN_TRAIN
			case eSelectPC::ONLY_CAN_TRAIN:
				if(univ.party[i].skill_pts){
					extra_info = std::to_string(univ.party[i].skill_pts) + " skill point";
					if(univ.party[i].skill_pts > 1) extra_info += "s";
				}else{
					extra_info = "no skill points";
					can_pick = false;
				}
				BOOST_FALLTHROUGH;
			case eSelectPC::ONLY_LIVING:
				if(univ.party[i].main_status != eMainStatus::ALIVE){
					can_pick = false;
					extra_info = "";
				}
				break;
			case eSelectPC::ONLY_STONE:
				if(univ.party[i].main_status != eMainStatus::STONE){
					can_pick = false;
					extra_info = "";
				}
				break;
			case eSelectPC::ONLY_DEAD:
				if(univ.party[i].main_status == eMainStatus::ALIVE){
					can_pick = false;
					extra_info = "";
				}
				break;
			case eSelectPC::ONLY_CAN_LOCKPICK:{
				if(univ.party[i].main_status != eMainStatus::ALIVE){
					can_pick = false;
					break;
				}
				else if(!univ.party[i].has_abil(eItemAbil::LOCKPICKS)){
					can_pick = false;
					extra_info = "no picks";
					break;
				}
				const cInvenSlot& pick_slot = univ.party[i].has_abil_equip(eItemAbil::LOCKPICKS);
				if(!pick_slot){
					can_pick = false;
					extra_info = "picks not equipped";
					break;
				}else{
					const cItem& picks = *pick_slot;
					std::string pick_name = picks.name;
					if(picks.ident) pick_name = picks.full_name;
					extra_info = pick_name + " x " + std::to_string(picks.charges);
				}
			}break;
			// Suppress a compiler warning:
			default:
				break;
		}
		selectPc["pc" + n].setText(univ.party[i].name);
		if(highlight_highest != eSkill::INVALID){
			selectPc["pc" + n].appendText(" ({{skill}})");
		}
		if(!extra_info.empty()){
			selectPc["pc" + n].appendText(": " + extra_info);
		}
		if(highlight_highest != eSkill::INVALID){
			short skill = univ.party[i].skills[highlight_highest];
			pc_skills[i] = skill;
			if(univ.party[i].is_alive()){
				if(skill > highest_skill) highest_skill = skill;
				if(skill != last_skill) all_pcs_equal = false;
				last_skill = skill;
			}
		}
		if(!can_pick) {
			selectPc["pick" + n].hide();
			if(extra_info.empty())
				selectPc["pc" + n].hide();
		} else {
			any_options = true;
		}
	}

	if(!any_options){
		if(mode == eSelectPC::ONLY_CAN_LOCKPICK){
			ASB("  No one has lockpicks equipped.");
			print_buf();
		}
		if(mode == eSelectPC::ONLY_CAN_TRAIN){
			ASB("  No one has skill points.");
			print_buf();
		}
		return 8;
	}

	if(highlight_highest != eSkill::INVALID){
		selectPc["hint"].replaceText("{{skill}}", get_str("skills", (int)highlight_highest * 2 + 1));

		for(int i = 0; i < 6; i++){
			std::string n = boost::lexical_cast<std::string>(i + 1);
			selectPc["pc" + n].replaceText("{{skill}}", std::to_string(pc_skills[i]));
			if(pc_skills[i] == highest_skill && !all_pcs_equal){
				selectPc["pc" + n].setColour(Colours::LIGHT_GREEN);
			}
		}
	}else{
		selectPc["hint"].hide();
	}

	if(!allow_choose_all){
		selectPc["pick-all"].hide();
		selectPc["all"].hide();
	}
	
	selectPc.run();
	item_hit = selectPc.getResult<short>();
	
	return item_hit;
}
