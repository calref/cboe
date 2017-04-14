
#include <cstdio>
#include <queue>

#include "boe.global.hpp"

#include "universe.hpp"

#include "boe.graphutil.hpp"
#include "boe.graphics.hpp"
#include "boe.newgraph.hpp"
#include "boe.fileio.hpp"
#include "boe.items.hpp"
#include "boe.monster.hpp"
#include "boe.town.hpp"
#include "boe.combat.hpp"
#include "boe.party.hpp"
#include "boe.text.hpp"
#include "sounds.hpp"
#include "boe.locutils.hpp"
#include "boe.specials.hpp"
#include "boe.infodlg.hpp"
#include "mathutil.hpp"
#include "boe.main.hpp"
#include "render_image.hpp"
#include "render_shapes.hpp"
#include "render_text.hpp"
#include "tiling.hpp"
#include "strdlog.hpp"
#include "fileio.hpp"
#include "winutil.hpp"
#include "res_image.hpp"

extern short stat_window,store_spell_target,which_combat_type,combat_active_pc;
extern eGameMode overall_mode;
extern location center;
extern sf::RenderWindow mainPtr;
extern short store_current_pc,current_ground;
extern eGameMode store_pre_shop_mode,store_pre_talk_mode;
extern std::queue<pending_special_type> special_queue;

extern bool map_visible;
extern sf::RenderWindow mini_map;

extern location hor_vert_place[14];
extern location diag_place[14];
extern location golem_m_locs[16];
extern cUniverse univ;
extern cCustomGraphics spec_scen_g;
bool need_map_full_refresh = true,forcing_map_button_redraw = false;
extern sf::RenderTexture map_gworld;
// In the 0..65535 range, this colour was {65535,65535,52428}
sf::Color parchment = {255,255,205};

long pause_dummy;

location town_map_adj;
short town_force = 200,store_which_shop,store_min,store_max,store_shop,store_selling_pc;
short sell_offset = 0;
location town_force_loc;
bool shop_button_active[12];
rectangle map_title_rect = {3,50,15,300};
rectangle map_bar_rect = {15,50,27,300};

void force_town_enter(short which_town,location where_start) {
	town_force = which_town;
	town_force_loc = where_start;
}

//short entry_dir; // if 9, go to forced
void start_town_mode(short which_town, short entry_dir) {
	short town_number;
	short at_which_save_slot,former_town;
	bool monsters_loaded = false,town_toast = false;
	location loc;
	unsigned short temp;
	bool play_town_sound = false;
	
	if(town_force < 200)
		which_town = town_force;
	else play_town_sound = true;
	
	former_town = town_number = which_town;
	
	if(town_number < 0 || town_number >= univ.scenario.towns.size()) {
		showError("The scenario tried to put you into a town that doesn't exist.",
			"Requested town: " + std::to_string(town_number) + "|Max town: " + std::to_string(univ.scenario.towns.size()));
		return;
	}
	
	// Now adjust town number as necessary.
	for(const auto& mod : univ.scenario.town_mods)
		if(mod.spec >= 0 && mod.spec < 200 && town_number == mod.spec && univ.party.sd_legit(mod.x, mod.y)) {
			former_town = town_number;
			town_number += PSD[mod.x][mod.y];
			// Now update horses & boats
			for(auto& horse : univ.party.horses)
				if(horse.exists && horse.which_town == former_town)
					horse.which_town = town_number;
			for(auto& boat : univ.party.boats)
				if(boat.exists && boat.which_town == former_town)
					boat.which_town = town_number;
		}
	
	
	
	if(town_number < 0 || town_number >= univ.scenario.towns.size()) {
		showError("The scenario tried to put you into a town that doesn't exist.",
			"Requested town: " + std::to_string(former_town) + "|Adjusted town: " + std::to_string(town_number) + "|Max town: " + std::to_string(univ.scenario.towns.size()));
		return;
	}
	
	overall_mode = MODE_TOWN;
	
	univ.party.town_num = town_number;
	
	if(play_town_sound) {
		if(univ.town->lighting_type > 0)
			play_sound(95);
		else play_sound(16);
	}

	// TODO: This means cleared webs reappear, for example. Is that right?
	univ.town.place_preset_fields();
	
	univ.town.belt_present = false;
	// Set up map, using stored map
	for(short i = 0; i < univ.town->max_dim; i++)
		for(short j = 0; j < univ.town->max_dim; j++) {
			if(univ.town->maps[j][i])
				make_explored(i,j);
			
			// TODO: Don't hard-code these ground types!
			if(univ.town->terrain(i,j) == 0)
				current_ground = 0;
			else if(univ.town->terrain(i,j) == 2)
				current_ground = 2;
			if(univ.scenario.ter_types[univ.town->terrain(i,j)].special == eTerSpec::CONVEYOR)
				univ.town.belt_present = true;
		}
	
	univ.town.monst.which_town = town_number;
	univ.town.monst.hostile = false;
	
	at_which_save_slot = univ.party.at_which_save_slot;
	
	for(auto& pop : univ.party.creature_save)
		if(town_number == pop.which_town) {
			univ.town.monst = pop;
			monsters_loaded = true;
			
			for(auto& monst : univ.town.monst) {
				if(loc_off_act_area(monst.cur_loc))
					monst.active = 0;
				if(monst.active == 2)
					monst.active = 1;
				monst.cur_loc = monst.start_loc;
				monst.health = monst.m_health;
				monst.mp = monst.max_mp;
				monst.morale = monst.m_morale;
				monst.status.clear();
				if(monst.summon_time > 0)
					monst.active = 0;
				monst.target = 6;
				// Bonus SP and HP wear off
				if(monst.mp > monst.max_mp)
					monst.mp = monst.max_mp;
				if(monst.health > monst.m_health)
					monst.health = monst.m_health;
			}
			
			// Now, travelling NPCs might have arrived. Go through and put them in.
			// These should have protected status (i.e. spec1 >= 200, spec1 <= 204)
			for(auto& monst : univ.town.monst) {
				switch(monst.time_flag){
					case eMonstTime::ALWAYS: break; // Nothing to do.
					case eMonstTime::SOMETIMES_A: case eMonstTime::SOMETIMES_B: case eMonstTime::SOMETIMES_C:
						if((univ.party.calc_day() % 3) + 3 != int(monst.time_flag))
							monst.active = 0;
						else {
							monst.active = 1;
							monst.spec_enc_code = 0;
							monst.cur_loc = monst.start_loc;
							monst.health = monst.m_health;
						}
						break ;
						
						// Now, appearing/disappearing monsters might have arrived/disappeared.
					case eMonstTime::APPEAR_ON_DAY:
						if(day_reached(monst.monster_time, monst.time_code)) {
							monst.active = 1;
							monst.time_flag = eMonstTime::ALWAYS;
						}
						break;
					case eMonstTime::DISAPPEAR_ON_DAY:
						if(day_reached(monst.monster_time, monst.time_code)) {
							monst.active = 0;
							monst.time_flag = eMonstTime::ALWAYS;
						}
						break;
					case eMonstTime::APPEAR_WHEN_EVENT:
						if(univ.party.key_times.find(monst.time_code) == univ.party.key_times.end())
							break; // Event hasn't happened yet
						if(univ.party.calc_day() >= univ.party.key_times[monst.time_code]){ //calc_day is used because of the "definition" of univ.party.key_times
							monst.active = 1;
							monst.time_flag = eMonstTime::ALWAYS;
						}
						break;
						
					case eMonstTime::DISAPPEAR_WHEN_EVENT:
						if(univ.party.key_times.find(monst.time_code) == univ.party.key_times.end())
							break; // Event hasn't happened yet
						if(univ.party.calc_day() >= univ.party.key_times[monst.time_code]){
							monst.active = 0;
							monst.time_flag = eMonstTime::ALWAYS;
						}
						break;
					case eMonstTime::APPEAR_AFTER_CHOP:
						// TODO: Should these two cases be separated?
						if(univ.town->town_chop_time > 0 && day_reached(univ.town->town_chop_time,univ.town->town_chop_key))
							monst.active += 10;
						else if(univ.town->is_cleaned_out())
							monst.active += 10;
						else monst.active = 0;
						if(monst.active >= 10)
							monst.time_flag = eMonstTime::ALWAYS;
						break;
				}
			}
			
			for(short j = 0; j < univ.town->max_dim; j++)
				for(short k = 0; k < univ.town->max_dim; k++) { // now load in saved setup,
					// except that pushable things restore to orig locs
					// TODO: THIS IS A TEMPORARY HACK TO GET i VALUE
					int i = std::find_if(univ.party.creature_save.begin(), univ.party.creature_save.end(), [&pop](cPopulation& p) {return &p == &pop;}) - univ.party.creature_save.begin();
					temp = univ.party.setup[i][j][k] << 8;
					temp &= ~(OBJECT_CRATE | OBJECT_BARREL | OBJECT_BLOCK);
					univ.town.fields[j][k] |= temp;
				}
		}
	
	if(!monsters_loaded) {
		univ.town.monst.clear();
		for(short i = 0; i < univ.town->creatures.size(); i++){
			if(univ.town->creatures[i].number > 0) {
				// First set up the values.
				const cTownperson& preset = univ.town->creatures[i];
				univ.town.monst.assign(i, preset, univ.scenario.scen_monsters[preset.number], univ.party.easy_mode, univ.difficulty_adjust());
				cCreature& monst = univ.town.monst[i];
				
				if(monst.spec_enc_code > 0)
					monst.active = 0;
				
				// Now, if necessary, fry the monster.
				switch(monst.time_flag) {
					case eMonstTime::APPEAR_ON_DAY:
						if(!day_reached(monst.monster_time, monst.time_code))
							monst.active = 0;
						break;
					case eMonstTime::DISAPPEAR_ON_DAY:
						if(day_reached(monst.monster_time, monst.time_code))
							monst.active = 0;
						break;
					case eMonstTime::SOMETIMES_A: case eMonstTime::SOMETIMES_B: case eMonstTime::SOMETIMES_C:
						if((univ.party.calc_day() % 3) + 3 != int(monst.time_flag)) {
							monst.active = 0;
						}
						else {
							monst.active = 1;
						}
						break;
					case eMonstTime::APPEAR_WHEN_EVENT:
						if(univ.party.key_times.find(monst.time_code) == univ.party.key_times.end())
							monst.active = 0; // Event hasn't happened yet
						else if(univ.party.calc_day() < univ.party.key_times[univ.town.monst[i].time_code])
							monst.active = 0; // This would only be reached if the time was set back (or in a legacy save)
						break;
						
					case eMonstTime::DISAPPEAR_WHEN_EVENT:
						if(univ.party.key_times.find(monst.time_code) == univ.party.key_times.end())
							break; // Event hasn't happened yet
						if(univ.party.calc_day() >= univ.party.key_times[univ.town.monst[i].time_code])
							monst.active = 0;
						break;
					case eMonstTime::APPEAR_AFTER_CHOP:
						// TODO: Should these two cases be separated?
						if(univ.town->town_chop_time > 0 && day_reached(univ.town->town_chop_time,univ.town->town_chop_key))
							monst.active += 10;
							else if(univ.town->is_cleaned_out())
							monst.active += 10;
						else monst.active = 0;
						break;
					case eMonstTime::ALWAYS:
						break;
				}
				
				if(monst.active) {
					// In forcecage?
					if(univ.town.is_force_cage(monst.cur_loc.x, monst.cur_loc.y))
						monst.status[eStatus::FORCECAGE] = 1000;
				}
			}
		}
	}
	
	// Now munch all large monsters that are misplaced
	// only large monsters, as some smaller monsters are intentionally placed
	// where they cannot be
	for(short i = 0; i < univ.town.monst.size(); i++) {
		if(univ.town.monst[i].active > 0)
			if(((univ.town.monst[i].x_width > 1) || (univ.town.monst[i].y_width > 1)) &&
				!monst_can_be_there(univ.town.monst[i].cur_loc,i))
				univ.town.monst[i].active = 0;
	}
	
	
	// Thrash town?
	if(univ.town->is_cleaned_out()) {
		town_toast = true;
		add_string_to_buf("Area has been cleaned out.");
	}
	if(univ.town->town_chop_time > 0) {
		if(day_reached(univ.town->town_chop_time,univ.town->town_chop_key)) {
			add_string_to_buf("Area has been abandoned.");
			for(auto& monst : univ.town.monst)
				if(monst.active > 0 && monst.active < 10 && !monst.is_friendly())
					monst.active += 10;
			town_toast = true;
		}
	}
	if(town_toast) {
		for(auto& monst : univ.town.monst)
			if(monst.active >= 10)
				monst.active -= 10;
			else monst.active = 0;
	}
	handle_town_specials(town_number, (short) town_toast,(entry_dir < 9) ? univ.town->start_locs[entry_dir] : town_force_loc);
	
	// Flush excess doomguards and viscous goos
	for(short i = 0; i < univ.town.monst.size(); i++)
		if((univ.town.monst[i].abil[eMonstAbil::SPLITS].active) &&
			(i >= univ.town->creatures.size() || univ.town.monst[i].number != univ.town->creatures[i].number))
			univ.town.monst[i].active = 0;
	
	// Set up field booleans, correct for doors
	for(short j = 0; j < univ.town->max_dim; j++)
		for(short k = 0; k < univ.town->max_dim; k++) {
			loc.x = j; loc.y = k;
			if(is_door(loc)) {
				univ.town.set_web(j,k,false);
				univ.town.set_crate(j,k,false);
				univ.town.set_barrel(j,k,false);
				univ.town.set_fire_barr(j,k,false);
				univ.town.set_force_barr(j,k,false);
				univ.town.set_quickfire(j,k,false);
			}
			if(univ.town.is_quickfire(j,k))
				univ.town.quickfire_present = true;
		}
	
	// Set up items, maybe place items already there
	univ.town.items.clear();
	
	for(short j = 0; j < 3; j++)
		if(univ.scenario.store_item_towns[j] == town_number) {
			univ.town.items = univ.party.stored_items[j];
		}
	
	for(short i = 0; i < univ.town->preset_items.size(); i++)
		if(univ.town->preset_items[i].code >= 0) {
			const cTown::cItem& preset = univ.town->preset_items[i];
			// place the preset item, if party hasn't gotten it already
			univ.town.items.push_back(univ.scenario.get_stored_item(preset.code));
			cItem& item = univ.town.items.back();
			item.item_loc = preset.loc;
			
			// Don't place special items if already in the party's possession
			if(item.variety == eItemType::SPECIAL && univ.party.spec_items.count(item.item_level)) {
				univ.town.items.pop_back();
				continue;
			}
			// Don't place quest items if party already started
			if(item.variety == eItemType::QUEST && univ.party.active_quests[item.item_level].status != eQuestStatus::AVAILABLE) {
				univ.town.items.pop_back();
				continue;
			}
			// Don't place the item if the party already took it, unless it's always there
			if(univ.town->item_taken[i] && !preset.always_there) {
				univ.town.items.pop_back();
				continue;
			}
			
			// Not use the items data flags, starting with forcing an ability
			if(preset.ability >= 0) {
				// TODO: What other ways might there be to use this?
				switch(item.variety) {
					case eItemType::ONE_HANDED:
					case eItemType::TWO_HANDED: {
						if(preset.ability > int(eEnchant::BLESSED))
							break;
						// TODO: This array and accompanying calculation is now duplicated here and in place_buy_button()
						const short aug_cost[10] = {4,7,10,8, 15,15,10, 0,0,0};
						int ench = preset.ability;
						int val = max(aug_cost[ench] * 100, item.value * (5 + aug_cost[ench]));
						item.enchant_weapon(eEnchant(ench), val);
						break;
					}
					default: break; // Silence compiler warning
				}
			}
			
			if(preset.charges > 0) {
				eItemType variety = item.variety;
				if(item.charges > 0)
					item.charges = preset.charges;
				else if(variety == eItemType::GOLD || variety == eItemType::FOOD)
					item.item_level = preset.charges;
			}
			
			if(town_toast)
				item.property = false;
			else
				item.property = preset.property;
			item.contained = preset.contained;
			int x = item.item_loc.x, y = item.item_loc.y;
			if(item.contained && (univ.town.is_barrel(x,y) || univ.town.is_crate(x,y)))
				item.held = true;
			item.is_special = i + 1;
		}
	
	
	for(auto& monst : univ.town.monst)
		if(loc_off_act_area(monst.cur_loc))
			monst.active = 0;
	for(auto& item : univ.town.items)
		if(loc_off_act_area(item.item_loc))
			item.variety = eItemType::NO_ITEM;
	
	// Clean out unwanted monsters
	for(auto& monst : univ.town.monst)
		if(univ.party.sd_legit(monst.spec1, monst.spec2)) {
			if(PSD[monst.spec1][monst.spec2] > 0)
				monst.active = 0;
		}
	
	erase_specials();
//	make_town_trim(0);
	
	
	univ.party.town_loc = (entry_dir < 9) ? univ.town->start_locs[entry_dir] : town_force_loc;
	center = univ.party.town_loc;
	if(univ.party.in_boat >= 0) {
		univ.party.boats[univ.party.in_boat].which_town = which_town;
		univ.party.boats[univ.party.in_boat].loc = univ.party.town_loc;
	}
	if(univ.party.in_horse >= 0) {
		univ.party.horses[univ.party.in_horse].which_town = which_town;
		univ.party.horses[univ.party.in_horse].loc = univ.party.town_loc;
	}
	
	
	// End flying
	if(univ.party.status[ePartyStatus::FLIGHT] > 0) {
		univ.party.status[ePartyStatus::FLIGHT] = 0;
		add_string_to_buf("You land, and enter.             ");
	}
	
	// No hostile monsters present.
	univ.party.hostiles_present = 0;
	
	add_string_to_buf("Now entering:");
	add_string_to_buf("   " + univ.town->name);
	
	
	// clear entry space, and check exploration
	univ.town.set_web(univ.party.town_loc.x,univ.party.town_loc.y,false);
	univ.town.set_crate(univ.party.town_loc.x,univ.party.town_loc.y,false);
	univ.town.set_barrel(univ.party.town_loc.x,univ.party.town_loc.y,false);
	univ.town.set_fire_barr(univ.party.town_loc.x,univ.party.town_loc.y,false);
	univ.town.set_force_barr(univ.party.town_loc.x,univ.party.town_loc.y,false);
	univ.town.set_quickfire(univ.party.town_loc.x,univ.party.town_loc.y,false);
	update_explored(univ.party.town_loc);
	
	// If a PC dead, drop his items
	for(cPlayer& pc : univ.party) {
		if(pc.main_status == eMainStatus::ALIVE || isSplit(pc.main_status))
			continue;
		for(cItem& item : pc.items)
			if(item.variety != eItemType::NO_ITEM) {
				place_item(item,univ.party.town_loc);
				item.variety = eItemType::NO_ITEM;
			}
	}
	
	for(auto& monst : univ.town.monst) {
		monst.targ_loc.x = 0;
		monst.targ_loc.y = 0;
	}
	
	// check horses
	for(short i = 0; i < univ.party.boats.size(); i++) {
		if(univ.scenario.boats[i].which_town >= 0 && univ.scenario.boats[i].loc.x >= 0) {
			if(!univ.party.boats[i].exists) {
				univ.party.boats[i] = univ.scenario.boats[i];
				univ.party.boats[i].exists = true;
			}
		}
	}
	for(short i = 0; i < univ.party.horses.size(); i++) {
		if(univ.scenario.horses[i].which_town >= 0 && univ.scenario.horses[i].loc.x >= 0) {
			if(!univ.party.horses[i].exists) {
				univ.party.horses[i] = univ.scenario.horses[i];
				univ.party.horses[i].exists = true;
			}
		}
	}
	
	clear_map();
	reset_item_max();
	town_force = 200;
	// TODO: One problem with this - it paints the terrain after the town entry dialog is dismissed
	// ... except it actually doesn't, because the town enter special is only queued, not run immediately.
	draw_terrain(1);
}


location end_town_mode(short switching_level,location destination) { // returns new party location
	location to_return;
	bool data_saved = false,combat_end = false;
	
	if(is_combat())
		combat_end = true;
	
	// Bonus SP and HP wear off
	for(short i = 0; i < 6; i++) {
		if(univ.party[i].cur_sp > univ.party[i].max_sp)
			univ.party[i].cur_sp = univ.party[i].max_sp;
		if(univ.party[i].cur_health > univ.party[i].max_health)
			univ.party[i].cur_health = univ.party[i].max_health;
	}
	
	if(overall_mode == MODE_TOWN) {
		for(auto& pop : univ.party.creature_save)
			if(pop.which_town == univ.party.town_num) {
				pop = univ.town.monst;
				// TODO: THIS IS A TEMPORARY HACK TO GET i VALUE
				int i = std::find_if(univ.party.creature_save.begin(), univ.party.creature_save.end(), [&pop](cPopulation& p) {return &p == &pop;}) - univ.party.creature_save.begin();
				for(short j = 0; j < univ.town->max_dim; j++)
					for(short k = 0; k < univ.town->max_dim; k++)
						univ.party.setup[i][j][k] = (univ.town.fields[j][k] & 0xff00) >> 8;
				data_saved = true;
			}
		if(!data_saved) {
			univ.party.creature_save[univ.party.at_which_save_slot] = univ.town.monst;
			for(short j = 0; j < univ.town->max_dim; j++)
				for(short k = 0; k < univ.town->max_dim; k++)
					univ.party.setup[univ.party.at_which_save_slot][j][k] = (univ.town.fields[j][k] & 0xff00) >> 8;
			univ.party.at_which_save_slot = (univ.party.at_which_save_slot == 3) ? 0 : univ.party.at_which_save_slot + 1;
		}
		
		// Store items, if necessary
		for(short j = 0; j < 3; j++)
			if(univ.scenario.store_item_towns[j] == univ.party.town_num) {
				univ.party.stored_items[j].clear();
				for(short i = 0; i < univ.town.items.size(); i++)
					if(univ.town.items[i].variety != eItemType::NO_ITEM && univ.town.items[i].is_special == 0 &&
							univ.town.items[i].item_loc.x >= univ.scenario.store_item_rects[j].left &&
							univ.town.items[i].item_loc.x <= univ.scenario.store_item_rects[j].right &&
							univ.town.items[i].item_loc.y >= univ.scenario.store_item_rects[j].top &&
							univ.town.items[i].item_loc.y <= univ.scenario.store_item_rects[j].bottom) {
						univ.party.stored_items[j].push_back(univ.town.items[i]);
					}
			}
		
		// Now store map
		for(short i = 0; i < univ.town->max_dim; i++)
			for(short j = 0; j < univ.town->max_dim; j++)
				if(is_explored(i,j)) {
					univ.town->maps[j].set(i);
				}
		
		to_return = univ.party.out_loc;
		
		auto& timers = univ.party.party_event_timers;
		timers.erase(std::remove_if(timers.begin(), timers.end(), [](const cTimer& t) {
			return t.node_type == 2;
		}), timers.end());
		
	}
	
	
	// Check for exit specials, if leaving town
	if(switching_level == 0) {
		to_return = univ.party.out_loc;
		
		if(is_town()) {
			if(destination.x <= univ.town->in_town_rect.left) {
				if(univ.town->exits[1].x > 0)
					to_return = local_to_global(univ.town->exits[1]);
				else to_return.x--;
				univ.party.out_loc = to_return; univ.party.out_loc.x++;
				handle_leave_town_specials(univ.party.town_num, univ.town->exits[1].spec,destination) ;
			}
			else if(destination.x >= univ.town->in_town_rect.right) {
				if(univ.town->exits[3].x > 0)
					to_return = local_to_global(univ.town->exits[3]);
				else to_return.x++;
				univ.party.out_loc = to_return; univ.party.out_loc.x--;
				handle_leave_town_specials(univ.party.town_num, univ.town->exits[3].spec,destination) ;
			}
			else if(destination.y <= univ.town->in_town_rect.top) {
				if(univ.town->exits[0].x > 0)
					to_return = local_to_global(univ.town->exits[0]);
				else to_return.y--;
				univ.party.out_loc = to_return; univ.party.out_loc.y++;
				handle_leave_town_specials(univ.party.town_num, univ.town->exits[0].spec,destination) ;
			}
			else if(destination.y >= univ.town->in_town_rect.bottom) {
				if(univ.town->exits[2].x > 0)
					to_return = local_to_global(univ.town->exits[2]);
				else to_return.y++;
				univ.party.out_loc = to_return; univ.party.out_loc.y--;
				handle_leave_town_specials(univ.party.town_num, univ.town->exits[2].spec,destination) ;
			}
			
		}
	}
	
	if(switching_level == 0) {
		overall_mode = MODE_OUTDOORS;
		
		erase_out_specials();
		
		univ.party.status[ePartyStatus::STEALTH] = 0;
		univ.party.status[ePartyStatus::DETECT_LIFE] = 0; // TODO: Yes? No? Maybe?
		for(cPlayer& who : univ.party)
			who.clear_brief_status();
		
		update_explored(to_return);
		redraw_screen(REFRESH_TERRAIN | REFRESH_TEXT);
		
	}
	
	if(!combat_end)
		clear_map();
	
	univ.party.town_num = 200; // should be harmless...
	
	return to_return;
}

void handle_town_specials(short /*town_number*/, bool town_dead,location /*start_loc*/) {
	queue_special(eSpecCtx::ENTER_TOWN, 2, town_dead ? univ.town->spec_on_entry_if_dead : univ.town->spec_on_entry, univ.party.town_loc);
}

void handle_leave_town_specials(short /*town_number*/, short which_spec,location /*start_loc*/) {
	queue_special(eSpecCtx::LEAVE_TOWN, 2, which_spec, univ.party.out_loc);
}

bool abil_exists(eItemAbil abil) { // use when outdoors
	for(const cPlayer& pc : univ.party)
		for(const cItem& item : pc.items)
			if(item.variety != eItemType::NO_ITEM && item.ability == abil)
				return true;
	for(short i = 0; i < 3; i++)
		for(short j = 0; j < univ.town.items.size(); j++)
			if(univ.party.stored_items[i][j].variety != eItemType::NO_ITEM && univ.party.stored_items[i][j].ability == abil)
				return true;
	
	return false;
}




void start_town_combat(eDirection direction) {
	place_party(direction);
	if(univ.cur_pc == 6)
		for(short i = 0; i < 6; i++)
			if(univ.party[i].main_status == eMainStatus::ALIVE) {
				univ.cur_pc = i;
				break;
			}
	center = univ.current_pc().combat_pos;
	
	which_combat_type = 1;
	overall_mode = MODE_COMBAT;
	
	combat_active_pc = 6;
	for(short i = 0; i < univ.town.monst.size(); i++)
		univ.town.monst[i].target = 6;
	
	for(short i = 0; i < 6; i++) {
		univ.party[i].last_attacked = nullptr;
		univ.party[i].parry = 0;
		univ.party[i].direction = direction;
		univ.current_pc().direction = direction;
		if(univ.party[i].main_status == eMainStatus::ALIVE)
			update_explored(univ.party[i].combat_pos);
	}
	
	store_current_pc = univ.cur_pc;
	univ.cur_pc = 0;
	set_pc_moves();
	pick_next_pc();
	center = univ.current_pc().combat_pos;
	draw_buttons(0);
	put_pc_screen();
	set_stat_window(univ.cur_pc);
	give_help(48,49);
	
}

eDirection end_town_combat() {
	short num_tries = 0,r1;
	int in_cage = 0;
	location cage_loc;
	bool same_cage = true;
	for(int i = 0; i < 6; i++) {
		if(univ.party[i].status[eStatus::FORCECAGE] > 0) {
			if(in_cage == 0)
				cage_loc = univ.party[i].get_loc();
			in_cage++;
		}
		if(univ.party[i].get_loc() != cage_loc)
			same_cage = false;
	}
	if(in_cage != 0 && in_cage != univ.party.count() && !same_cage) {
		add_string_to_buf("  Someone trapped.");
		return DIR_HERE;
	}
	// TODO: Don't allow ending combat if two PCs are separated by an impassable wall with no way around it
	
	r1 = get_ran(1,0,5);
	while(univ.party[r1].main_status != eMainStatus::ALIVE && num_tries++ < 1000)
		r1 = get_ran(1,0,5);
	univ.party.town_loc = univ.party[r1].combat_pos;
	overall_mode = MODE_TOWN;
	univ.cur_pc = store_current_pc;
	if(univ.current_pc().main_status != eMainStatus::ALIVE)
		univ.cur_pc = first_active_pc();
	for(short i = 0; i < 6; i++) {
		univ.party[i].parry = 0;
		univ.party[i].combat_pos = {-1,-1};
	}
	return univ.party[r1].direction;
}

void place_party(short direction) {
	bool in_cage = false;
	for(int n = 0; n < 6; n++)
		if(univ.party[n].status[eStatus::FORCECAGE])
			in_cage = true;
	
	if(in_cage) {
		for(int n = 0; n < 6; n++)
			univ.party[n].combat_pos = univ.party.town_loc;
		return;
	}
	
	bool spot_ok[14] = {true,true,true,true,true,true,true,
		true,true,true,true,true,true,true};
	location pos_locs[14];
	location check_loc;
	short x_adj,y_adj,how_many_ok = 1,where_in_a = 0;
	
	for(short i = 0; i < 14; i++) {
		check_loc = univ.party.town_loc;
		if(direction % 4 < 2)
			x_adj = ((direction % 2 == 0) ? hor_vert_place[i].x : diag_place[i].x);
		else x_adj = ((direction % 2 == 0) ? hor_vert_place[i].y : diag_place[i].y);
		if(direction % 2 == 0)
			x_adj = (direction < 4) ? x_adj : -1 * x_adj;
		else x_adj = ((direction == 1) || (direction == 7)) ? -1 * x_adj : x_adj;
		check_loc.x -= x_adj;
		if(direction % 4 < 2)
			y_adj = ((direction % 2 == 0) ? hor_vert_place[i].y : diag_place[i].y);
		else y_adj = ((direction % 2 == 0) ? hor_vert_place[i].x : diag_place[i].x);
		if(direction % 2 == 0)
			y_adj = ((direction > 1) && (direction < 6)) ? y_adj : -1 * y_adj;
		else y_adj = ((direction == 3) || (direction == 1)) ? -1 * y_adj : y_adj;
		
		check_loc.y -= y_adj;
		pos_locs[i] = check_loc;
		if(!is_blocked(check_loc) && !is_special(check_loc) && sight_obscurity(check_loc.x,check_loc.y) == 0
		   && can_see_light(univ.party.town_loc,check_loc,combat_obscurity) < 1 && !loc_off_act_area(check_loc)) {
			spot_ok[i] = true;
			how_many_ok += (i > 1) ? 1 : 0;
		}
		else spot_ok[i] = false;
		
		if(i == 0)
			spot_ok[i] = true;
	}
	for(short i = 0; i < 6; i++) {
		if(univ.party[i].main_status == eMainStatus::ALIVE) {
			if(how_many_ok == 1)
				univ.party[i].combat_pos = pos_locs[where_in_a];
			else {
				univ.party[i].combat_pos = pos_locs[where_in_a];
				if(how_many_ok > 1)
					where_in_a++;
				how_many_ok--;
//				if(how_many_ok > 1) {
				while(!spot_ok[where_in_a] && where_in_a < 14)
					where_in_a++;
//				}
			}
		}
	}
}

void create_out_combat_terrain(short ter_type,short num_walls,bool is_road) {
	short r1,arena;
	static const short ter_base[20] = {
		2,0,36,50,71,     0,0,0, 0,2,
		2,2, 2, 0, 0,    36,0,2, 0,2,
	};
	static const location special_ter_locs[15] = {
		loc(11,10),loc(11,14),loc(10,20),loc(11,26),loc(9,30),
		loc(15,19),loc(23,19),loc(19,29),loc(20,11),loc(28,16),
		loc(28,24),loc(27,19),loc(27,29),loc(15,28),loc(19,19),
	};
	static const ter_num_t cave_pillar[4][4] = {
		{0 ,14,11,1 },
		{14,19,20,11},
		{17,18,21,8 },
		{1 ,17,8 ,0 }
	};
	static const ter_num_t mntn_pillar[4][4] = {
		{37,29,27,36},
		{29,33,34,27},
		{31,32,35,25},
		{36,31,25,37}
	};
	static const ter_num_t surf_lake[4][4] = {
		{56,55,54,3 },
		{57,50,61,54},
		{58,51,59,53},
		{3 ,4 ,58,52}
	};
	static const ter_num_t cave_lake[4][4] = {
		{93,96,71,71},
		{96,71,71,71},
		{71,71,71,96},
		{71,71,71,96}
	};
	static const ter_num_t surf_fume[4][4] = {
		{75,75,75,36},
		{75,75,75,75},
		{75,75,75,75},
		{36,37,75,75}
	};
	static const ter_num_t cave_fume[4][4] = {
		{98,0 ,75,75},
		{0 ,75,75,75},
		{75,75,75,0 },
		{75,75,75,0 }
	};
	// TODO: There's no surface bonfire or bedding in the default terrain set!
	static const ter_num_t surf_camp[4][4] = {
		{105,  2,4,  2},
		{  2,104,2,105},
		{115,  2,4,  2},
		{  2,105,2,115},
	};
	static const ter_num_t cave_camp[4][4] = {
		{105,  0, 1,  0},
		{  0,104, 0,105},
		{ 92,  0,93,  0},
		{  0,105, 0, 92},
	};
	static const short terrain_odds[20][10] = {
		{ 3, 80,    4, 40,   115, 20,   114, 10,   112,  1},	// Grassy field
		{ 1, 50,   93, 25,    94,  5,    98, 10,    95,  1},	// Ordinary cave
		{37, 20},												// Mountain
		{64,  3,   63,  1},										// Surface bridge
		{74,  1},												// Cave bridge
		{84,700,   97, 30,    98, 20,    92,  4,    95,  1},	// Rubble-strewn cave
		{93,280,   91,300,    92,270,    95,  7,    98, 10},	// Cave forest
		{ 1,800,   93,600,    94, 10,    92, 10,    95,  4},	// Cave mushrooms
		{ 1,700,   96,200,    95,100,    92, 10,   112,  5},	// Cave swamp
		{ 3,600,   87, 90,   110, 20,   114,  6,   113,  2},	// Surface rocks
		{ 3,200,    4,400,   111,250},							// Surface swamp
		{ 3,200,    4,300,   112, 50,   113, 60,   114,100},	// Surface woods
		{ 3,100,    4,250,   115,120,   114, 30,   112,  2},	// Surface shrubbery
		{ 1, 25,   76, 15,    98,300,    97,280,    75,  5},	// Stalagmites
		{ 1,150,   94, 80,    98, 20,    76, 20,    75,  5},	// Cave fumarole
		{37,150,   76, 20,    75,  5},							// Surface fumarole
		{ 1, 50,   93, 25,    94,  5,    98, 10,    95,  1},	// Cave camp
		{ 3, 80,    4, 40,   115, 20,   114, 10,   112,  1},	// Surface camp
		{ 1,600,   97, 50,    98, 80,    93, 10,    84, 10},	// Cave crops
		{ 3,500,    4,500,   110, 50,    87, 10},				// Surface crops
	}; // ter then odds then ter then odds ...
	// These sets only include the ones that can have a road running through them
	static const std::set<int> cave_arenas = {1,5,6,7,8,13};
	static const std::set<int> surface_arenas = {0,2,9,10,11,12};
	location stuff_ul;
	
	arena = univ.scenario.ter_types[ter_type].combat_arena;
	if(arena >= 1000) {
		arena -= 1000;
		// We take the terrain from the specified town, and nothing else.
		// No preset creatures, items, special nodes, etc.
		// Furthermore, if it's a large town, we drop the outer 8 tiles.
		size_t town_size = univ.scenario.towns[arena]->max_dim;
		int offset = max(0,town_size - 48);
		rectangle town_bounds = univ.scenario.towns[arena]->in_town_rect;
		// Just in case the town boundary is somehow larger than the town...
		town_bounds.left = minmax(0,town_size - 1, town_bounds.left);
		town_bounds.right = minmax(0,town_size - 1, town_bounds.right);
		town_bounds.top = minmax(0,town_size - 1, town_bounds.top);
		town_bounds.bottom = minmax(0,town_size - 1, town_bounds.bottom);
		for(short i = 0; i < 48; i++)
			for(short j = 0; j < 48; j++) {
				// This test also accounts for small towns since the town boundary is never larger than the town
				if(town_bounds.contains(i + offset, j + offset))
					univ.town->terrain(i,j) = univ.scenario.towns[arena]->terrain(i + offset,j + offset);
				else univ.town->terrain(i,j) = 90;
			}
		// The game uses the upper left corner to replace spaces that are blocked, so it needs to be set to something sensible.
		// We'll take the terrain at the first entry location.
		location fetch = univ.scenario.towns[arena]->start_locs[0];
		univ.town->terrain(0,0) = univ.scenario.towns[arena]->terrain(fetch.x, fetch.y);
		// And now we're done, so skip the rest of this function.
		return;
	}
	
	for(short i = 0; i < 48; i++)
		for(short j = 0; j < 48; j++) {
			univ.town.fields[i][j] = 0;
			if((j <= 8) || (j >= 35) || (i <= 8) || (i >= 35))
				univ.town->terrain(i,j) = 90;
			else univ.town->terrain(i,j) = ter_base[arena];
		}
	for(short i = 0; i < 48; i++)
		for(short j = 0; j < 48; j++)
			for(short k = 0; k < 5; k++)
				if((univ.town->terrain(i,j) != 90) && (get_ran(1,1,1000) < terrain_odds[arena][k * 2 + 1]))
					univ.town->terrain(i,j) = terrain_odds[arena][k * 2];
	
	univ.town->terrain(0,0) = ter_base[arena];
	
	bool is_bridge = (arena == 3 || arena == 4);
	
	if(arena == 3 || (is_road && surface_arenas.count(arena))) {
		univ.town->terrain(0,0) = 83;
		for(short i = (is_bridge ? 15 : 19); i < (is_bridge ? 26 : 23); i++)
			for(short j = 9; j < 35; j++)
				univ.town->terrain(i,j) = 83;
	}
	if(arena == 4 || (is_road && cave_arenas.count(arena))) {
		univ.town->terrain(0,0) = 82;
		for(short i = (is_bridge ? 15 : 19); i < (is_bridge ? 26 : 23); i++)
			for(short j = 9; j < 35; j++)
				univ.town->terrain(i,j) = 82;
	}
	if(arena == 18 || arena == 19) {
		for(short i = 12; i < 15; i++)
			for(short j = 9; j < 35; j++)
				if(j != 17 && j != 26)
					univ.town->terrain(i,j) = ter_type;
		for(short i = 17; i < 20; i++)
			for(short j = 9; j < 35; j++)
				if(j != 17 && j != 26)
					univ.town->terrain(i,j) = ter_type;
		for(short i = 22; i < 25; i++)
			for(short j = 9; j < 35; j++)
				if(j != 17 && j != 26)
					univ.town->terrain(i,j) = ter_type;
		for(short i = 27; i < 30; i++)
			for(short j = 9; j < 35; j++)
				if(j != 17 && j != 26)
					univ.town->terrain(i,j) = ter_type;
	}
	if(arena == 14 || arena == 15)
		univ.town->terrain(0,0) = 75;
	
	
	// Now place special lakes, etc.
	if(arena == 2 || arena == 15)
		for(short i = 0; i < 15; i++)
			if(get_ran(1,0,5) == 1) {
				stuff_ul = special_ter_locs[i];
				for(short j = 0; j < 4; j++)
					for(short k = 0; k < 4; k++)
						univ.town->terrain(stuff_ul.x + j,stuff_ul.y + k) = mntn_pillar[k][j];
			}
	if(univ.town->terrain(0,0) == 0)
		for(short i = 0; i < 15; i++)
			if(get_ran(1,0,25) == 1) {
				stuff_ul = special_ter_locs[i];
				for(short j = 0; j < 4; j++)
					for(short k = 0; k < 4; k++)
						univ.town->terrain(stuff_ul.x + j,stuff_ul.y + k) = cave_pillar[k][j];
			}
	if(univ.town->terrain(0,0) == 0)
		for(short i = 0; i < 15; i++)
			if(get_ran(1,0,40) == 1) {
				stuff_ul = special_ter_locs[i];
				for(short j = 0; j < 4; j++)
					for(short k = 0; k < 4; k++)
						univ.town->terrain(stuff_ul.x + j,stuff_ul.y + k) = cave_lake[k][j];
			}
	if(univ.town->terrain(0,0) == 2)
		for(short i = 0; i < 15; i++)
			if(get_ran(1,0,40) == 1) {
				stuff_ul = special_ter_locs[i];
				for(short j = 0; j < 4; j++)
					for(short k = 0; k < 4; k++)
						univ.town->terrain(stuff_ul.x + j,stuff_ul.y + k) = surf_lake[k][j];
			}
	if(arena == 14)
		for(short i = 0; i < 15; i++)
			if(get_ran(1,0,5) == 1) {
				stuff_ul = special_ter_locs[i];
				for(short j = 0; j < 4; j++)
					for(short k = 0; k < 4; k++)
						univ.town->terrain(stuff_ul.x + j,stuff_ul.y + k) = cave_fume[k][j];
			}
	if(arena == 15)
		for(short i = 0; i < 15; i++)
			if(get_ran(1,0,5) == 1) {
				stuff_ul = special_ter_locs[i];
				for(short j = 0; j < 4; j++)
					for(short k = 0; k < 4; k++)
						univ.town->terrain(stuff_ul.x + j,stuff_ul.y + k) = surf_fume[k][j];
			}
	if(arena == 16) {
		stuff_ul = loc(18,14);
		for(short j = 0; j < 4; j++)
			for(short k = 0; k < 4; k++)
				univ.town->terrain(stuff_ul.x + j,stuff_ul.y + k) = cave_camp[k][j];
	}
	if(arena == 17) {
		stuff_ul = loc(18,14);
		for(short j = 0; j < 4; j++)
			for(short k = 0; k < 4; k++)
				univ.town->terrain(stuff_ul.x + j,stuff_ul.y + k) = surf_camp[k][j];
	}
	
	
	if(ter_base[ter_type] == 0) {
		for(short i = 0; i < num_walls; i++) {
			r1 = get_ran(1,0,3);
			for(short j = 9; j < 35; j++)
				switch(r1) {
					case 0:
						univ.town->terrain(j,8) = 6;
						break;
					case 1:
						univ.town->terrain(8,j) = 9;
						break;
					case 2:
						univ.town->terrain(j,35) = 12;
						break;
					case 3:
						univ.town->terrain(32,j) = 15;
						break;
				}
		}
		if((univ.town->terrain(17,8) == 6) && (univ.town->terrain(8,20) == 9))
			univ.town->terrain(8,8) = 21;
		if((univ.town->terrain(32,20) == 15) && (univ.town->terrain(17,35) == 12))
			univ.town->terrain(32,35) = 19;
		if((univ.town->terrain(17,8) == 6) && (univ.town->terrain(32,20) == 15))
			univ.town->terrain(32,8) = 32;
		if((univ.town->terrain(8,20) == 9) && (univ.town->terrain(17,35) == 12))
			univ.town->terrain(8,35) = 20;
	}
	if(ter_base[ter_type] == 36) {
		for(short i = 0; i < num_walls; i++) {
			r1 = get_ran(1,0,3);
			for(short j = 9; j < 35; j++)
				switch(r1) {
					case 0:
						univ.town->terrain(j,8) = 24;
						break;
					case 1:
						univ.town->terrain(8,j) = 26;
						break;
					case 2:
						univ.town->terrain(j,35) = 28;
						break;
					case 3:
						univ.town->terrain(32,j) = 30;
						break;
				}
		}
		if((univ.town->terrain(17,8) == 6) && (univ.town->terrain(8,20) == 9))
			univ.town->terrain(8,8) = 35;
		if((univ.town->terrain(32,20) == 15) && (univ.town->terrain(17,35) == 12))
			univ.town->terrain(32,35) = 33;
		if((univ.town->terrain(17,8) == 6) && (univ.town->terrain(32,20) == 15))
			univ.town->terrain(32,8) = 32;
		if((univ.town->terrain(8,20) == 9) && (univ.town->terrain(17,35) == 12))
			univ.town->terrain(8,35) = 34;
	}
	
	// TODO: Looks like I haven't yet implemented the generalized arenas
	
//	make_town_trim(1);
}


void elim_monst(unsigned short which,short spec_a,short spec_b) {
	if(!univ.party.sd_legit(spec_a,spec_b))
		return;
	if(PSD[spec_a][spec_b] > 0) {
		for(short i = 0; i < univ.town.monst.size(); i++)
			if(univ.town.monst[i].number == which) {
				univ.town.monst[i].active = 0;
			}
	}
	
}



//short print_mes; // 0 - no 1 - yes
void dump_gold(short print_mes) {
	// Mildly kludgy gold check
	if(univ.party.gold > 30000) {
		univ.party.gold = 30000;
		if(print_mes == 1) {
			put_pc_screen();
			add_string_to_buf("Excess gold dropped.");
			print_buf();
		}
	}
	if(univ.party.food > 25000) {
		univ.party.food = 25000;
		if(print_mes == 1) {
			put_pc_screen();
			add_string_to_buf("Excess food dropped.");
			print_buf();
		}
	}
}



void pick_lock(location where,short pc_num) {
	ter_num_t terrain;
	short r1;
	bool will_break = false;
	short unlock_adjust;
	
	terrain = univ.town->terrain(where.x,where.y);
	cInvenSlot which_item = univ.party[pc_num].has_abil_equip(eItemAbil::LOCKPICKS);
	if(!which_item) {
		add_string_to_buf("  Need lockpick equipped.");
		return;
	}
	
	r1 = get_ran(1,1,100) + which_item->abil_data[0] * 7;
	
	if(r1 < 75)
		will_break = true;
	
	r1 = get_ran(1,1,100) - 5 * univ.party[pc_num].stat_adj(eSkill::DEXTERITY) + univ.town.difficulty * 7
		- 5 * univ.party[pc_num].skill(eSkill::LOCKPICKING) - which_item->abil_data[0] * 7;
	
	// Nimble?
	if(univ.party[pc_num].traits[eTrait::NIMBLE])
		r1 -= 8;
	
	if(univ.party[pc_num].has_abil_equip(eItemAbil::THIEVING))
		r1 = r1 - 12;
	
	if(univ.scenario.ter_types[terrain].special != eTerSpec::UNLOCKABLE) {
		add_string_to_buf("  Wrong terrain type.");
		return;
	}
	unlock_adjust = univ.scenario.ter_types[terrain].flag2;
	if((unlock_adjust >= 5) || (r1 > (unlock_adjust * 15 + 30))) {
		add_string_to_buf("  Didn't work.");
		if(will_break) {
			add_string_to_buf("  Pick breaks.");
			univ.party[pc_num].remove_charge(which_item.slot);
			if(stat_window == pc_num)
				put_item_screen(stat_window);
		}
		play_sound(41);
	}
	else {
		add_string_to_buf("  Door unlocked.");
		play_sound(9);
		univ.town->terrain(where.x,where.y) = univ.scenario.ter_types[terrain].flag1;
	}
}

void bash_door(location where,short pc_num) {
	ter_num_t terrain;
	short r1,unlock_adjust;
	
	terrain = univ.town->terrain(where.x,where.y);
	r1 = get_ran(1,1,100) - 15 * univ.party[pc_num].stat_adj(eSkill::STRENGTH) + univ.town.difficulty * 4;
	
	if(univ.scenario.ter_types[terrain].special != eTerSpec::UNLOCKABLE) {
		add_string_to_buf("  Wrong terrain type.");
		return;
	}
	
	unlock_adjust = univ.scenario.ter_types[terrain].flag2;
	if(unlock_adjust >= 5 || r1 > (unlock_adjust * 15 + 40) || univ.scenario.ter_types[terrain].flag3 != 1)  {
		add_string_to_buf("  Didn't work.");
		damage_pc(univ.party[pc_num],get_ran(1,1,4),eDamageType::SPECIAL,eRace::UNKNOWN,0);
	}
	else {
		add_string_to_buf("  Lock breaks.");
		play_sound(9);
		univ.town->terrain(where.x,where.y) = univ.scenario.ter_types[terrain].flag1;
	}
}


void erase_specials() {
	location where;
	short sd1,sd2;
	cSpecial sn;
	
	if((is_combat()) && (which_combat_type == 0))
		return;
	if(!is_town() && !is_combat())
		return;
	for(short k = 0; k < univ.town->special_locs.size(); k++) {
		if(univ.town->special_locs[k].spec < 0 || univ.town->special_locs[k].spec >= univ.town->specials.size())
			continue;
		sn = univ.town->specials[univ.town->special_locs[k].spec];
		sd1 = sn.sd1; sd2 = sn.sd2;
		if((univ.party.sd_legit(sd1,sd2)) && (PSD[sd1][sd2] == 250)) {
			long spec = univ.town->special_locs[k].spec;
			where = univ.town->special_locs[k];
			if(spec >= 0 && (where.x > univ.town->max_dim || where.y > univ.town->max_dim || where.x < 0 || where.y < 0)) {
				beep();
				add_string_to_buf("Town corrupt. Problem fixed.");
				print_nums(where.x,where.y,k);
				univ.town->special_locs[k].spec = -1;
			}
			
			if(spec >= 0) {
				univ.town.set_spot(where.x,where.y,false);
			}
		}
	}
}

void erase_out_specials() {
	
	short out_num;
	cSpecial sn;
	short sd1,sd2;
	location where;
	
	for(short i = 0; i < 2; i++)
		for(short j = 0; j < 2; j++)
			if(quadrant_legal(i,j)) {
				cOutdoors& sector = *univ.scenario.outdoors[univ.party.outdoor_corner.x + i][univ.party.outdoor_corner.y + j];
				for(short k = 0; k < sector.city_locs.size(); k++) {
					if(sector.city_locs[k].spec >= 0 && sector.city_locs[k].x < 48 &&
						univ.scenario.ter_types[sector.terrain[sector.city_locs[k].x][sector.city_locs[k].y]].special == eTerSpec::TOWN_ENTRANCE &&
					   (sector.city_locs[k].x == minmax(0,47,sector.city_locs[k].x)) &&
					   (sector.city_locs[k].y == minmax(0,47,sector.city_locs[k].y))) {
						if(sector.city_locs[k].spec < 0 || sector.city_locs[k].spec >= univ.scenario.towns.size())
							continue;
						if(!univ.scenario.towns[sector.city_locs[k].spec]->can_find) {
							univ.out[48 * i + sector.city_locs[k].x][48 * j + sector.city_locs[k].y] =
								univ.scenario.ter_types[sector.terrain[sector.city_locs[k].x][sector.city_locs[k].y]].flag1;
						}
						else if(univ.scenario.towns[sector.city_locs[k].spec]->can_find) {
							univ.out[48 * i + sector.city_locs[k].x][48 * j + sector.city_locs[k].y] =
								sector.terrain[sector.city_locs[k].x][sector.city_locs[k].y];
							
						}
					}
				}
				for(int k = 0; k < sector.special_locs.size(); k++) {
					if(sector.special_locs[k].spec < 0) continue; // TODO: Is this needed? Seems to be important, so why was it commented out?
					out_num = univ.scenario.outdoors.width() * (univ.party.outdoor_corner.y + j) + univ.party.outdoor_corner.x + i;
					
					sn = sector.specials[sector.special_locs[k].spec];
					sd1 = sn.sd1; sd2 = sn.sd2;
					if((univ.party.sd_legit(sd1,sd2)) && (PSD[sd1][sd2] == 250)) {
						where = sector.special_locs[k];
						if(where.x > 48 || where.y > 48 || where.x < 0 || where.y < 0) {
							beep();
							add_string_to_buf("Outdoor section corrupt. Problem fixed.");
							sector.special_locs[k].spec = -1; // TODO: Again, was there a reason for commenting this out?
						}
						
						sector.special_spot[where.x][where.y] = false;
					}
				}
			}
}

// TODO: I don't think we need this
void clear_map() {
	rectangle map_world_rect(map_gworld);
	
//	if(!map_visible) {
//		return;
//	}
//	draw_map(mini_map,11);
	
	fill_rect(map_gworld, map_world_rect, sf::Color::Black);
	
}

void draw_map(bool need_refresh) {
	if(!map_visible) return;
	pic_num_t pic;
	rectangle the_rect,map_world_rect = {0,0,384,384};
	location where;
	location kludge;
	rectangle draw_rect,orig_draw_rect = {0,0,6,6},ter_temp_from,base_source_rect = {0,0,12,12};
	rectangle	dlogpicrect = {6,6,42,42};
	bool draw_pcs = true,out_mode;
	rectangle view_rect= {0,0,48,48},tiny_rect = {0,0,32,32},
	redraw_rect = {0,0,48,48},big_rect = {0,0,64,64}; // Rectangle visible in view screen
	
	rectangle area_to_draw_from,area_to_draw_on = {29,47,269,287};
	ter_num_t what_ter;
	bool draw_surroundings = false,expl;
	short total_size = 48; // if full redraw, use this to figure out everything
	rectangle custom_from;
	
	draw_surroundings = true;
	
	town_map_adj.x = 0;
	town_map_adj.y = 0;
	
	// view rect is rect that is visible, redraw rect is area to redraw now
	// area_to_draw_from is final draw from rect
	// area_to_draw_on is final draw to rect
	// extern short store_pre_shop_mode,store_pre_talk_mode;
	if((is_out()) || ((is_combat()) && (which_combat_type == 0)) ||
		((overall_mode == MODE_TALKING) && (store_pre_talk_mode == 0)) ||
		((overall_mode == MODE_SHOPPING) && (store_pre_shop_mode == 0))) {
		view_rect.left = minmax(0,8,univ.party.loc_in_sec.x - 20);
		view_rect.right = view_rect.left + 40;
		view_rect.top = minmax(0,8,univ.party.loc_in_sec.y - 20);
		view_rect.bottom = view_rect.top + 40;
		redraw_rect = view_rect;
	}
	else {
		total_size = univ.town->max_dim;
		switch(total_size) {
			case 64:
				view_rect.left = minmax(0,24,univ.party.town_loc.x - 20);
				view_rect.right = view_rect.left + 40;
				view_rect.top = minmax(0,24,univ.party.town_loc.y - 20);
				view_rect.bottom = view_rect.top + 40;
				redraw_rect = big_rect;
				break;
			case 48:
				view_rect.left = minmax(0,8,univ.party.town_loc.x - 20);
				view_rect.right = view_rect.left + 40;
				view_rect.top = minmax(0,8,univ.party.town_loc.y - 20);
				view_rect.bottom = view_rect.top + 40;
				redraw_rect = view_rect;
				break;
			case 32:
				view_rect = tiny_rect;
				redraw_rect = view_rect;
				break;
		}
	}
	if((is_out()) || ((is_combat()) && (which_combat_type == 0)) ||
		((overall_mode == MODE_TALKING) && (store_pre_talk_mode == 0)) ||
		((overall_mode == MODE_SHOPPING) && (store_pre_shop_mode == 0)) ||
		is_town() || is_combat()) {
		area_to_draw_from = view_rect;
		area_to_draw_from.width() = 40;
		area_to_draw_from.height() = 40;
		area_to_draw_from.left *= 6;
		area_to_draw_from.right *= 6;
		area_to_draw_from.top *= 6;
		area_to_draw_from.bottom *= 6;
	}
	
	if(is_combat())
		draw_pcs = false;
	
	const char* title_string = "Your map:";
	bool canMap = true;
	
	if((is_combat()) && (which_combat_type == 0)) {
		title_string = "No map in combat.";
		canMap = false;
	}else if((is_town() && univ.town->defy_mapping)) {
		title_string = "This place defies mapping.";
		canMap = false;
	}
	else if(need_refresh) {
		map_gworld.setActive();
		
		fill_rect(map_gworld, map_world_rect, sf::Color::Black);
		
		// Now, if shopping or talking, just don't touch anything.
		if((overall_mode == MODE_SHOPPING) || (overall_mode == MODE_TALKING))
			redraw_rect.right = -1;
		
		if((is_out()) ||
			((is_combat()) && (which_combat_type == 0)) ||
			((overall_mode == MODE_TALKING) && (store_pre_talk_mode == 0)) ||
			((overall_mode == MODE_SHOPPING) && (store_pre_shop_mode == 0)))
			out_mode = true;
		else out_mode = false;
		
		// TODO: It could be possible to draw the entire map here and then only refresh if a spot actually changes terrain type
		sf::Texture& small_ter_gworld = *ResMgr::get<ImageRsrc>("termap");
		for(where.x = redraw_rect.left; where.x < redraw_rect.right; where.x++)
			for(where.y = redraw_rect.top; where.y < redraw_rect.bottom; where.y++) {
				draw_rect = orig_draw_rect;
				draw_rect.offset(6 * where.x, 6 * where.y);
				
				if(out_mode)
					what_ter = univ.out[where.x + 48 * univ.party.i_w_c.x][where.y + 48 * univ.party.i_w_c.y];
				else what_ter = univ.town->terrain(where.x,where.y);
				
				ter_temp_from = base_source_rect;
				
				if(out_mode)
					expl = univ.out.out_e[where.x + 48 * univ.party.i_w_c.x][where.y + 48 * univ.party.i_w_c.y];
				else expl = is_explored(where.x,where.y);
				
				if(expl != 0) {
					pic = univ.scenario.ter_types[what_ter].map_pic;
					bool drawLargeIcon = false;
					if(pic == NO_PIC) {
						pic = univ.scenario.ter_types[what_ter].picture;
						drawLargeIcon = true;
					}
					if(pic >= 1000) {
						if(spec_scen_g) {
							//print_nums(0,99,pic);
							sf::Texture* src_gw;
							if(drawLargeIcon) {
								pic = pic % 1000;
								graf_pos_ref(src_gw, custom_from) = spec_scen_g.find_graphic(pic);
								rect_draw_some_item(*src_gw,custom_from,map_gworld,draw_rect);
							} else {
								graf_pos_ref(src_gw, custom_from) = spec_scen_g.find_graphic(pic % 1000);
								custom_from.right = custom_from.left + 12;
								custom_from.bottom = custom_from.top + 12;
								pic /= 1000; pic--;
								custom_from.offset((pic / 3) * 12, (pic % 3) * 12);
								rect_draw_some_item(*src_gw, custom_from, map_gworld, draw_rect);
							}
						}
					} else if(drawLargeIcon) {
						if(pic >= 960) {
							custom_from = calc_rect(4 * ((pic - 960) / 5),(pic - 960) % 5);
							rect_draw_some_item(*ResMgr::get<ImageRsrc>("teranim"), custom_from, map_gworld, draw_rect);
						} else {
							int which_sheet = pic / 50;
							sf::Texture* src_gw = ResMgr::get<ImageRsrc>("ter" + std::to_string(1 + which_sheet)).get();
							pic %= 50;
							custom_from = calc_rect(pic % 10, pic / 10);
							rect_draw_some_item(*src_gw, custom_from, map_gworld, draw_rect);
						}
					} else {
						if(univ.scenario.ter_types[what_ter].picture < 960)
							ter_temp_from.offset(12 * (univ.scenario.ter_types[what_ter].picture % 20),
												 12 * (univ.scenario.ter_types[what_ter].picture / 20));
						else ter_temp_from.offset(12 * 20,
												  12 * (univ.scenario.ter_types[what_ter].picture - 960));
						rect_draw_some_item(small_ter_gworld,ter_temp_from,map_gworld,draw_rect);
					}
					
					if(is_out() ? univ.out->roads[where.x][where.y] : univ.town.is_road(where.x,where.y)) {
						draw_rect.inset(1,1);
						rect_draw_some_item(*ResMgr::get<ImageRsrc>("trim"),{8,112,12,116},map_gworld,draw_rect);
					}
				}
			}
		
		map_gworld.display();
	}
	
	mini_map.setActive();
	
	// Now place terrain map gworld
	TextStyle style;
	style.font = FONT_BOLD;
	style.pointSize = 10;;
	
	the_rect = rectangle(mini_map);
	tileImage(mini_map, the_rect,bg[4]);
	cPict theGraphic(mini_map);
	theGraphic.setBounds(dlogpicrect);
	theGraphic.setPict(21, PIC_DLOG);
	theGraphic.setFormat(TXT_FRAME, false);
	theGraphic.draw();
	style.colour = sf::Color::White;
	style.lineHeight = 12;
	win_draw_string(mini_map, map_title_rect,title_string,eTextMode::WRAP,style);
	win_draw_string(mini_map, map_bar_rect,"(Hit Escape to close.)",eTextMode::WRAP,style);
	
	if(canMap) {
		rect_draw_some_item(map_gworld.getTexture(),area_to_draw_from,mini_map,area_to_draw_on);
		
		// Now place PCs and monsters
		if(draw_pcs) {
			if((is_town()) && (univ.party.status[ePartyStatus::DETECT_LIFE] > 0))
				for(short i = 0; i < univ.town.monst.size(); i++)
					if(univ.town.monst[i].active > 0) {
						where = univ.town.monst[i].cur_loc;
						if((is_explored(where.x,where.y)) &&
							((where.x >= view_rect.left) && (where.x < view_rect.right)
							 && where.y >= view_rect.top && where.y < view_rect.bottom)){
								
								draw_rect.left = area_to_draw_on.left + 6 * (where.x - view_rect.left);
								draw_rect.top = area_to_draw_on.top + 6 * (where.y - view_rect.top);
								draw_rect.right = draw_rect.left + 6;
								draw_rect.bottom = draw_rect.top + 6;
								
								fill_rect(mini_map, draw_rect, sf::Color::Green);
								frame_circle(mini_map, draw_rect, sf::Color::Blue);
							}
					}
			if((overall_mode != MODE_SHOPPING) && (overall_mode != MODE_TALKING)) {
				where = (is_town()) ? univ.party.town_loc : global_to_local(univ.party.out_loc);
				
				draw_rect.left = area_to_draw_on.left + 6 * (where.x - view_rect.left);
				draw_rect.top = area_to_draw_on.top + 6 * (where.y - view_rect.top);
				draw_rect.right = draw_rect.left + 6;
				draw_rect.bottom = draw_rect.top + 6;
				fill_rect(mini_map, draw_rect, sf::Color::Red);
				frame_circle(mini_map, draw_rect, sf::Color::Black);
				
			}
		}
	}
	
	mini_map.display();
	
	// Now exit gracefully
	mainPtr.setActive();
	
}



bool is_door(location destination) {
	
	if(univ.scenario.ter_types[univ.town->terrain(destination.x,destination.y)].special == eTerSpec::UNLOCKABLE ||
	   univ.scenario.ter_types[univ.town->terrain(destination.x,destination.y)].special == eTerSpec::CHANGE_WHEN_STEP_ON)
		return true;
	return false;
}


void display_map() {
	// Show the automap if it's not already visible
	if(map_visible) return;
	rectangle the_rect;
	rectangle	dlogpicrect = {6,6,42,42};
	
	mini_map.setVisible(true);
	map_visible = true;
	draw_map(true);
	makeFrontWindow(mainPtr);
}

void check_done() {
}

bool quadrant_legal(short i, short j) {
	if(univ.party.outdoor_corner.x + i >= univ.scenario.outdoors.width())
		return false;
	if(univ.party.outdoor_corner.y + j >= univ.scenario.outdoors.height())
		return false;
	if(univ.party.outdoor_corner.x + i < 0)
		return false;
	if(univ.party.outdoor_corner.y + j < 0)
		return false;
	return true;
}
