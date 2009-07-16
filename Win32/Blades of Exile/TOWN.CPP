#include <windows.h>
#include "stdio.h"
#include "global.h"
#include "gutils.h"
#include "graphics.h"
#include "newgraph.h"
#include "fileio.h"
#include "items.h"
#include "itemdata.h"
#include "monster.h"
#include "town.h"
#include "combat.h"
#include "party.h"
#include "text.h"
#include "exlsound.h"
#include "fields.h"
#include "locutils.h"
#include "dlogtool.h"
#include "infodlgs.h"
#include "graphutl.h"

#include "globvar.h"

void force_town_enter(short which_town,location where_start)
{
	town_force = which_town;
	town_force_loc = where_start;
}

void start_town_mode(short which_town, short entry_dir)
//short entry_dir; // if 9, go to forced
{
	short i,m,n;

	char message[60];
	short j,k,town_number;
	short at_which_save_slot,former_town;
	location loc;
	unsigned char temp;
	Boolean monsters_loaded = FALSE, town_toast = FALSE;
	Boolean play_town_sound = FALSE;

	if (town_force < INVALID_TOWN) which_town = town_force;
	else play_town_sound = TRUE;

	former_town = town_number = which_town;

	if ((town_number < 0) || (town_number >= scenario.num_towns))
	{
		char text[256];
		wsprintf(text, "town number = %i, max = %i, %i, %i", town_number,
		 scenario.num_towns, sizeof(short), sizeof(WORD));
		give_error("The scenario tried to put you into a town that doesn't exist.",text,0);
		return;
	}		

	// Now adjust town number as necessary.
	for (i = 0; i < 10; i++)
		if ((scenario.town_to_add_to[i] >= 0) && (scenario.town_to_add_to[i] < 200) &&
			(town_number == scenario.town_to_add_to[i]) &&
			(sd_legit(scenario.flag_to_add_to_town[i][0],scenario.flag_to_add_to_town[i][1]) == TRUE))
		{
			former_town = town_number;
			town_number += PSD[scenario.flag_to_add_to_town[i][0]][scenario.flag_to_add_to_town[i][1]];
			
			// Now update horses & boats
			for (i = 0; i < 30; i++) 	
				if ((party.horses[i].exists == TRUE) && (party.horses[i].which_town == former_town))
					party.horses[i].which_town = town_number;
					
			for (i = 0; i < 30; i++) 	
				if ((party.boats[i].exists == TRUE) && (party.boats[i].which_town == former_town))
					party.boats[i].which_town = town_number;
		}

	if ((town_number < 0) || (town_number >= scenario.num_towns))
	{
		give_error("The scenario tried to put you into a town that doesn't exist.","2",0);
		return;
	}
		
	overall_mode = MODE_TOWN;

	load_town(town_number,0,0,NULL);

	c_town.town_num = town_number;

	if (play_town_sound == TRUE)
	{
		if (c_town.town.lighting > 0) play_sound(95);
		else play_sound(16);
	}

	belt_present = FALSE;
	// Set up map, using stored map
	for (i = 0; i < town_size[town_type]; i++)
		for (j = 0; j < town_size[town_type]; j++) {
			c_town.explored[i][j] = 0;
				if (town_maps.town_maps[c_town.town_num][i / 8][j] & (char)(s_pow(2,i % 8)))
					make_explored(i,j);
					
/*			if (t_d.terrain[i][j] == 0)	current_ground = 0;
			else if (t_d.terrain[i][j] == 2) current_ground = 2;*/
			
			if ((scenario.ter_types[t_d.terrain[i][j]].special >= 16) &&
				(scenario.ter_types[t_d.terrain[i][j]].special <= 19))
					belt_present = TRUE;	
		}
	c_town.hostile = 0;
	c_town.monst.which_town = town_number;
	c_town.monst.friendly = 0;

	at_which_save_slot = party.at_which_save_slot;
	
	for (i = 0; i < 4; i++)
		if (town_number == party.creature_save[i].which_town) {
			c_town.monst = party.creature_save[i];
			monsters_loaded = TRUE;
						
			for (j = 0; j < T_M; j++) {
				if (loc_off_act_area(c_town.monst.dudes[j].m_loc) == TRUE)
					c_town.monst.dudes[j].active = 0;
				if (c_town.monst.dudes[j].active == 2)
					c_town.monst.dudes[j].active = 1;
				c_town.monst.dudes[j].m_loc = t_d.creatures[j].start_loc;
				c_town.monst.dudes[j].m_d.health = c_town.monst.dudes[j].m_d.m_health;
				c_town.monst.dudes[j].m_d.mp = c_town.monst.dudes[j].m_d.max_mp; 			
				c_town.monst.dudes[j].m_d.morale = c_town.monst.dudes[j].m_d.m_morale;
				for (k = 0; k < 15; k++)
					c_town.monst.dudes[j].m_d.status[k] = 0;
				if (c_town.monst.dudes[j].summoned > 0)
					c_town.monst.dudes[j].active = 0;
				monst_target[j] = 6;
				}
			
			// Now, travelling NPCs might have arrived. Go through and put them in.
			// These should have protected status (i.e. spec1 >= 200, spec1 <= 204)
			for (j = 0; j < T_M; j++) {
			switch (c_town.monst.dudes[j].monst_start.time_flag){
             case 4: case 5 : case 6:
						if ((((short) (party.age / 1000) % 3) + 4) != c_town.monst.dudes[j].monst_start.time_flag)
							c_town.monst.dudes[j].active = 0;
							else {
								c_town.monst.dudes[j].active = 1;
								c_town.monst.dudes[j].monst_start.spec_enc_code = 0;
								// Now remove time flag so it doesn't get reappearing
								c_town.monst.dudes[j].monst_start.time_flag = 0;
								c_town.monst.dudes[j].m_loc = t_d.creatures[j].start_loc;
								c_town.monst.dudes[j].m_d.health = c_town.monst.dudes[j].m_d.m_health;
								}
					break ;
					
			// Now, appearing/disappearing monsters might have arrived/disappeared.
            case 1:
                    if (day_reached(c_town.monst.dudes[j].monst_start.monster_time, c_town.monst.dudes[j].monst_start.time_code) == TRUE)
                        {
                        	c_town.monst.dudes[j].active = 1;
                        	c_town.monst.dudes[j].monst_start.time_flag=0; // Now remove time flag so it doesn't get reappearing
                        }
               	break;
            case 2:
                if (day_reached(c_town.monst.dudes[j].monst_start.monster_time, c_town.monst.dudes[j].monst_start.time_code) == TRUE)
                        {
                        	c_town.monst.dudes[j].active = 0;
                        	c_town.monst.dudes[j].monst_start.time_flag=0; // Now remove time flag so it doesn't get disappearing again
                        }
                break;
            case 7:
				if (calc_day() >= party.key_times[c_town.monst.dudes[j].monst_start.time_code]){ //calc_day is used because of the "definition" of party.key_times
                            c_town.monst.dudes[j].active = 1;
                            c_town.monst.dudes[j].monst_start.time_flag=0; // Now remove time flag so it doesn't get reappearing
                    }
				break;

			case 8:
    			if (calc_day() >= party.key_times[c_town.monst.dudes[j].monst_start.time_code]){
						c_town.monst.dudes[j].active = 0;
					    c_town.monst.dudes[j].monst_start.time_flag=0; // Now remove time flag so it doesn't get disappearing again
                    }
				break;
            case 0:
                break;
            default:
                break;
    }
}
			
			for (j = 0; j < town_size[town_type]; j++)
				for (k = 0; k < town_size[town_type]; k++) { // now load in saved setup, 
						// except that barrels and crates restore to orig locs
					temp = setup_save.setup[i][j][k] & 231;

					misc_i[j][k] = (misc_i[j][k] & 24) | temp;//setup_save.setup[i][j][k];
					}
			}

	if (monsters_loaded == FALSE) {
				for (i = 0; i < T_M; i++)
					if (t_d.creatures[i].number == 0) {
						c_town.monst.dudes[i].active = 0;
						c_town.monst.dudes[i].number = 0;
						c_town.monst.dudes[i].monst_start.time_flag = 0;
						c_town.monst.dudes[i].m_loc.x = 80;
						c_town.monst.dudes[i].monst_start.spec_enc_code = 0;
						}
					else {
						// First set up the values.
						monst_target[i] = 6;
						c_town.monst.dudes[i].active = 1;
						c_town.monst.dudes[i].number = t_d.creatures[i].number;
						c_town.monst.dudes[i].attitude = t_d.creatures[i].start_attitude;
						c_town.monst.dudes[i].m_loc = t_d.creatures[i].start_loc;
						c_town.monst.dudes[i].mobile = t_d.creatures[i].mobile;
						c_town.monst.dudes[i].m_d = return_monster_template(c_town.monst.dudes[i].number);

						c_town.monst.dudes[i].summoned = 0;
						c_town.monst.dudes[i].monst_start = t_d.creatures[i];

						if (c_town.monst.dudes[i].monst_start.spec_enc_code > 0)
							c_town.monst.dudes[i].active = 0;

						// Now, if necessary, fry the monster.
						switch (c_town.monst.dudes[i].monst_start.time_flag) {
							case 1:
								if (day_reached(c_town.monst.dudes[i].monst_start.monster_time,
								  c_town.monst.dudes[i].monst_start.time_code) == FALSE)
									c_town.monst.dudes[i].active = 0;
								break;
							case 2:
								if (day_reached(c_town.monst.dudes[i].monst_start.monster_time,
								  c_town.monst.dudes[i].monst_start.time_code) == TRUE)
									c_town.monst.dudes[i].active = 0;
								break;
							case 3:
								// unused
								break;
							case 4: case 5: case 6:
								if ((((short) (party.age / 1000) % 3) + 4) != c_town.monst.dudes[i].monst_start.time_flag) {
									c_town.monst.dudes[i].active = 0;
									c_town.monst.dudes[i].monst_start.spec_enc_code = 50;
									}
									else {
										c_town.monst.dudes[i].active = 1;
										// Now remove time flag so it doesn't keep reappearing
										c_town.monst.dudes[i].monst_start.time_flag = 0;
										}
								break;
							case 7:
								if (calc_day() < party.key_times[c_town.monst.dudes[i].monst_start.time_code])
									c_town.monst.dudes[i].active = 0;
								break;

							case 8:
								if (calc_day() >= party.key_times[c_town.monst.dudes[i].monst_start.time_code])
									c_town.monst.dudes[i].active = 0;
								break;
							case 9:
								if (c_town.town.town_chop_time > 0)
									if (day_reached(c_town.town.town_chop_time,c_town.town.town_chop_key) == TRUE) {
										c_town.monst.dudes[i].active += 10;
										break;
										}
								c_town.monst.dudes[i].active = 0;
								break;
							case 0:
								break;
							default:
								break;							}
					}
		}

	// Now munch all large monsters that are misplaced
	// only large monsters, as some smaller monsters are intentionally placed
	// where they cannot be
	for (i = 0; i < T_M; i++) {
		if (c_town.monst.dudes[i].active > 0)
			if (((c_town.monst.dudes[i].m_d.x_width > 1) || (c_town.monst.dudes[i].m_d.y_width > 1)) && 
				(monst_can_be_there(c_town.monst.dudes[i].m_loc,i) == FALSE))
				c_town.monst.dudes[i].active = 0;			
		}
	// Thrash town?
	if (party.m_killed[c_town.town_num] > c_town.town.max_num_monst) {
		town_toast = TRUE;
		add_string_to_buf("Area has been cleaned out.");
		}
	if (c_town.town.town_chop_time > 0) {
		if (day_reached(c_town.town.town_chop_time,c_town.town.town_chop_key) == TRUE){
			//add_string_to_buf("Area has been abandoned.");
			for (i = 0; i < T_M; i++)
				if ((c_town.monst.dudes[i].active > 0) && (c_town.monst.dudes[i].active < 10) && 
				(c_town.monst.dudes[i].attitude % 2 == 1))
					c_town.monst.dudes[i].active += 10;
			town_toast = TRUE;
			}
		}
	if (town_toast == TRUE) {
			for (i = 0; i < T_M; i++)
				if (c_town.monst.dudes[i].active >= 10)
					c_town.monst.dudes[i].active -= 10;
					else c_town.monst.dudes[i].active = 0;			
		}
	if ((short) town_toast > 0)
		special_queue[0].queued_special = c_town.town.spec_on_entry_if_dead;
		else special_queue[0].queued_special = c_town.town.spec_on_entry;
	// Flush excess doomguards and viscous goos
	for (i = 0; i < T_M; i++)
		if ((c_town.monst.dudes[i].m_d.spec_skill == 12) &&
			(c_town.monst.dudes[i].number != t_d.creatures[i].number))
				c_town.monst.dudes[i].active = 0;
		
	quickfire = FALSE;
	crate = FALSE;
	barrel = FALSE;
	web = FALSE;
	fire_barrier = FALSE;
	force_barrier = FALSE;
	// Set up field booleans, correct for doors
			for (j = 0; j < town_size[town_type]; j++)
				for (k = 0; k < town_size[town_type]; k++) {
					loc.x = j; loc.y = k;
					if (loc.isDoor()) misc_i[j][k] = misc_i[j][k] & 3;

					// Set up field booleans
					if (is_web(j,k) == TRUE) web = TRUE;
					if (is_crate(j,k) == TRUE) crate = TRUE;
					if (is_barrel(j,k) == TRUE) barrel = TRUE;
					if (is_fire_barrier(j,k) == TRUE) fire_barrier = TRUE;
					if (is_force_barrier(j,k) == TRUE) force_barrier = TRUE;
					if (is_quickfire(j,k) == TRUE) quickfire = TRUE;
					}
		
	// Set up items, maybe place items already there
	for (i = 0; i < NUM_TOWN_ITEMS; i++)
		t_i.items[i] = return_dummy_item();
							
	for (j = 0; j < 3; j++)
		if (scenario.store_item_towns[j] == town_number) {
			for (i = 0; i < NUM_TOWN_ITEMS; i++)
				t_i.items[i] = stored_items[j].items[i];		
			}				

	for (i = 0; i < 64; i++)
		if ((c_town.town.preset_items[i].item_code >= 0) 
		 && (((party.item_taken[c_town.town_num][i / 8] & s_pow(2,i % 8)) == 0) ||
		 (c_town.town.preset_items[i].always_there == TRUE))) {
			for (j = 0; j < NUM_TOWN_ITEMS; j++) 
			
				// place the preset item, if party hasn't gotten it already
				if (t_i.items[j].variety == 0) {
					t_i.items[j] = get_stored_item(c_town.town.preset_items[i].item_code);
					t_i.items[j].item_loc = c_town.town.preset_items[i].item_loc;

					// Not use the items data flags, starting with forcing an ability
					if (c_town.town.preset_items[i].ability >= 0) {
		 				switch (t_i.items[j].variety) {
							case 3: case 11: // If gold or food, this value is amount
								if (c_town.town.preset_items[i].ability > 0)
									t_i.items[j].item_level = c_town.town.preset_items[i].ability;
								break;
		 					}
		 				}
		 				
					if (c_town.town.preset_items[i].property == TRUE)
		 				t_i.items[j].item_properties = t_i.items[j].item_properties | 2;
		 			if (town_toast == TRUE) 
		 				t_i.items[j].item_properties = t_i.items[j].item_properties & 253;
		 			if (c_town.town.preset_items[i].contained == TRUE)
		 				t_i.items[j].item_properties = t_i.items[j].item_properties | 8;
		 			t_i.items[j].is_special = i + 1;
		 			
					j = NUM_TOWN_ITEMS;
					}
		 	}

							
	for (i = 0; i < T_M; i++) 
		if (loc_off_act_area(c_town.monst.dudes[i].m_loc) == TRUE)
			c_town.monst.dudes[i].active = 0;
	for (i = 0; i < NUM_TOWN_ITEMS; i++)
		if (loc_off_act_area(t_i.items[i].item_loc) == TRUE)
				t_i.items[i].variety = 0;		
				
	// Clean out unwanted monsters
	for (i = 0; i < T_M; i++) 
		if (sd_legit(c_town.monst.dudes[i].monst_start.spec1,c_town.monst.dudes[i].monst_start.spec2) == TRUE) {
			if (party.stuff_done[c_town.monst.dudes[i].monst_start.spec1][c_town.monst.dudes[i].monst_start.spec2] > 0)
				c_town.monst.dudes[i].active = 0;
			}
		
	erase_specials();
	make_town_trim(0);

	c_town.p_loc = (entry_dir < 9) ? c_town.town.start_locs[entry_dir] : town_force_loc;
	center = c_town.p_loc;
	if (party.in_boat >= 0) {
		party.boats[party.in_boat].which_town = which_town;
		party.boats[party.in_boat].boat_loc = c_town.p_loc;
		}
	if (party.in_horse >= 0) {
		party.horses[party.in_horse].which_town = which_town;
		party.horses[party.in_horse].horse_loc = c_town.p_loc;
		}
		
	// End flying
	if (party.stuff_done[305][1] > 0)
	{
		party.stuff_done[305][1] = 0;	
		add_string_to_buf("You land, and enter.             ");
	}

	// No hostile monsters present.
	party.stuff_done[305][9] = 0;
		
	add_string_to_buf("Now entering:");
	sprintf ((char *) message, "   %-30.30s ",data_store->town_strs[0]);
	add_string_to_buf((char *) message);                       

	// clear entry space, and check exploration
	misc_i[c_town.p_loc.x][c_town.p_loc.y] = misc_i[c_town.p_loc.x][c_town.p_loc.y] & 3;
	update_explored(c_town.p_loc);

	// If a PC dead, drop his items
	for (m = 0; m < 6; m++)
		for (n = 0; n < 24; n++)
			if ((adven[m].isAlive() == false) && (adven[m].items[n].variety != 0)) {
				place_item(adven[m].items[n],c_town.p_loc,TRUE);
				adven[m].items[n].variety = 0;
				}

	for (i = 0; i < T_M; i++)
		{monster_targs[i].x = 0;  monster_targs[i].y = 0;}

	//// check horses
	for (i = 0; i < 30; i++) {
		if ((scenario.scen_boats[i].which_town >= 0) && (scenario.scen_boats[i].boat_loc.x >= 0)) {
			if (party.boats[i].exists == FALSE) {
				party.boats[i] = scenario.scen_boats[i];
				party.boats[i].exists = TRUE;
				}
			}
		if ((scenario.scen_horses[i].which_town >= 0) && (scenario.scen_horses[i].horse_loc.x >= 0)) {
			if (party.horses[i].exists == FALSE) {
				party.horses[i] = scenario.scen_horses[i];
				party.horses[i].exists = TRUE;
				}
			}
		}

	// Place correct graphics
	redraw_screen(0);

	clear_map();
	reset_item_max();
	town_force = INVALID_TOWN;
}

location end_town_mode(short switching_level,location destination)  // returns new party location
{
	location to_return;
	short i,j,k;
	Boolean data_saved = FALSE,combat_end = FALSE;

	if (is_combat()) combat_end = TRUE;
	
	if (overall_mode == 1) {
			for (i = 0; i < 4; i++)
				if (party.creature_save[i].which_town == c_town.town_num) {
					party.creature_save[i] = c_town.monst;
					for (j = 0; j < town_size[town_type]; j++)
						for (k = 0; k < town_size[town_type]; k++)
							setup_save.setup[i][j][k] = misc_i[j][k];
					data_saved = TRUE;
					}
			if (data_saved == FALSE) {
				party.creature_save[party.at_which_save_slot] = c_town.monst;
				for (j = 0; j < town_size[town_type]; j++)
					for (k = 0; k < town_size[town_type]; k++)
						setup_save.setup[party.at_which_save_slot][j][k] = misc_i[j][k];
				party.at_which_save_slot = (party.at_which_save_slot == 3) ? 0 : party.at_which_save_slot + 1;
				} 

	// Store items, if necessary 
		for (j = 0; j < 3; j++)
			if (scenario.store_item_towns[j] == c_town.town_num) {
			for (i = 0; i < NUM_TOWN_ITEMS; i++)
				if ((t_i.items[i].variety != 0) && (t_i.items[i].is_special == 0) &&
				((t_i.items[i].item_loc.x >= scenario.store_item_rects[j].left) &&
				 (t_i.items[i].item_loc.x <= scenario.store_item_rects[j].right) && 
				 (t_i.items[i].item_loc.y >= scenario.store_item_rects[j].top) &&
				 (t_i.items[i].item_loc.y <= scenario.store_item_rects[j].bottom)) ) {
				 	stored_items[j].items[i] = t_i.items[i];
				 	}
				 	else stored_items[j].items[i].variety = 0;			
				}
				
		// Clean up special data, just in case
		for (i = 0; i < T_M; i++) {
			c_town.monst.dudes[i].monst_start.spec1 = 0;
			c_town.monst.dudes[i].monst_start.spec2 = 0;
			}

		// Now store map
			for (i = 0; i < town_size[town_type]; i++)
				for (j = 0; j < town_size[town_type]; j++)
					if (is_explored(i,j) > 0) {
							town_maps.town_maps[c_town.town_num][i / 8][j] = town_maps.town_maps[c_town.town_num][i / 8][j] |
								(char) (s_pow(2,i % 8));

					}
		
		to_return = party.p_loc;

		for (i = 0; i < 30; i++)
	 		if ((party.party_event_timers[i] > 0) && (party.global_or_town[i] == 1))
				party.party_event_timers[i] = 0;
		}
		
			
				// Check for exit specials, if leaving town
	if (switching_level == 0) {
		to_return = party.p_loc;

		if (is_town()) {
			if (destination.x <= c_town.town.in_town_rect.left) {
				if (c_town.town.exit_locs[1].x > 0) 
					to_return = c_town.town.exit_locs[1].toGlobal();
					else to_return.x--;
				party.p_loc = to_return; party.p_loc.x++;
				special_queue[1].queued_special = c_town.town.exit_specs[1];
				}
			else if (destination.x >= c_town.town.in_town_rect.right) {
				if (c_town.town.exit_locs[3].x > 0) 
					to_return = c_town.town.exit_locs[3].toGlobal();
					else to_return.x++;
				party.p_loc = to_return; party.p_loc.x--;
				special_queue[1].queued_special = c_town.town.exit_specs[3];
				}
			else if (destination.y <= c_town.town.in_town_rect.top) {
				if (c_town.town.exit_locs[0].x > 0) 
					to_return = c_town.town.exit_locs[0].toGlobal();
					else to_return.y--;
				party.p_loc = to_return; party.p_loc.y++;
				special_queue[1].queued_special = c_town.town.exit_specs[0];
				}
			else if (destination.y >= c_town.town.in_town_rect.bottom) {
				if (c_town.town.exit_locs[2].x > 0) 
					to_return = c_town.town.exit_locs[2].toGlobal();
					else to_return.y++;			
				party.p_loc = to_return; party.p_loc.y--;
				special_queue[1].queued_special = c_town.town.exit_specs[2];
				}

			}
		}

	if (switching_level == 0)
	{
		fix_boats();
		overall_mode = 0;

		erase_out_specials();


		party.stuff_done[305][0] = 0;
			for (i = 0; i < 6; i++)
				for (j = 0; j < 15; j++)
					if ((j != 2) && (j != 7) && (j != 9))
						adven[i].status[j] = 0;
	
		update_explored(to_return);
		redraw_screen(0);
	}

	if (combat_end == FALSE) clear_map();
	
    c_town.town_num = -1; //no longer in a town
	
	return to_return;
}

void start_town_combat(short direction)
{
	short i;

	create_town_combat_terrain();

	place_party(direction);

	if (current_pc == 6) {
		for (i = 0; i < 6; i++)
			if (adven[i].isAlive()) {
				current_pc = i;
				i = 6;
            }
		}
	center = pc_pos[current_pc];

	which_combat_type = 1;
	overall_mode = MODE_COMBAT;

	combat_active_pc = 6;
	for (i = 0; i < T_M; i++)
		monst_target[i] = 6;

	for (i = 0; i < 6; i++) {
		last_attacked[i] = T_M + 10;
		pc_parry[i] = 0;
		pc_dir[i] = direction;
		adven[current_pc].direction = direction;
		if (adven[i].isAlive())
			update_explored(pc_pos[i]);
		}

	store_current_pc = current_pc;
	current_pc = 0;
	set_pc_moves();
	pick_next_pc();
	center = pc_pos[current_pc];
	draw_buttons(0);
	put_pc_screen();
	set_stat_window(current_pc);
	give_help(48,49,0);

}

short end_town_combat()
{
	short num_tries = 0,r1,i;
	
	r1 = get_ran(1,0,5);
	while ((adven[r1].isAlive() == false) && (num_tries++ < 1000))
			r1 = get_ran(1,0,5);
	c_town.p_loc = pc_pos[r1];
	overall_mode = 1;
	current_pc = store_current_pc;
	if (adven[current_pc].isAlive() == false)
		current_pc = first_active_pc();
	for (i = 0; i < 6; i++) 
		pc_parry[i] = 0;
	return pc_dir[r1];
}

void place_party(short direction)
{
	Boolean spot_ok[14] = {TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,
							TRUE,TRUE,TRUE,TRUE,TRUE,TRUE,TRUE};
	location pos_locs[14];
	location check_loc;
	short x_adj,y_adj,how_many_ok = 1,where_in_a = 0,i;
	
	for (i = 0; i < 14; i++) {
		check_loc = c_town.p_loc;
		if (direction % 4 < 2)
			x_adj = ((direction % 2 == 0) ? hor_vert_place[i].x : diag_place[i].x);
			else x_adj = ((direction % 2 == 0) ? hor_vert_place[i].y : diag_place[i].y);
		if (direction % 2 == 0)
			x_adj = (direction < 4) ? x_adj : -1 * x_adj;
			else x_adj = ((direction == 1) || (direction == 7)) ? -1 * x_adj : x_adj;
		check_loc.x -= x_adj;
		if (direction % 4 < 2)
			y_adj = ((direction % 2 == 0) ? hor_vert_place[i].y : diag_place[i].y);
			else y_adj = ((direction % 2 == 0) ? hor_vert_place[i].x : diag_place[i].x);
		if (direction % 2 == 0)
			y_adj = ((direction > 1) && (direction < 6)) ? y_adj : -1 * y_adj;
			else y_adj = ((direction == 3) || (direction == 1)) ? -1 * y_adj : y_adj;
		
		check_loc.y -= y_adj;		
		pos_locs[i] = check_loc;
		if ((loc_off_act_area(check_loc) == FALSE) &&
			(is_blocked(check_loc) == FALSE) && (is_special(check_loc) == FALSE) && (get_obscurity(check_loc.x,check_loc.y) == 0)
			&& (can_see(c_town.p_loc,check_loc,1) < 1)) {
			spot_ok[i] = TRUE;
			how_many_ok += (i > 1) ? 1 : 0;
			}
			else spot_ok[i] = FALSE;

		if (i == 0)
			spot_ok[i] = TRUE;
		}
	i = 0;
	while (i < 6) {
		if (adven[i].isAlive()) {
				if (how_many_ok == 1)
					pc_pos[i] = pos_locs[where_in_a];
					else {
						pc_pos[i] = pos_locs[where_in_a];
						if (how_many_ok > 1)
							where_in_a++;
						how_many_ok--;
							while (spot_ok[where_in_a] == FALSE)
								where_in_a++;
						}
			}
		i++;	
		}
}



void create_town_combat_terrain()
{
	location where;
	
	for (where.x = 0; where.x < town_size[town_type]; where.x++)
		for (where.y = 0; where.y < town_size[town_type]; where.y++) 
					combat_terrain[where.x][where.y] = t_d.terrain[where.x][where.y];
}

void create_out_combat_terrain(short type,short num_walls)
// spec_code is encounter's spec_code
{
	short i,j,k,r1,ter_type;
// 0 grass 1 cave 2 mntn 3 bridge 4 cave bridge 5 rubble cave 6 cave tree 7 cave mush
// 8 cave swamp 9 surface rocks 10 surface swamp 11 surface woods 12 s. shrub 13 stalags
// 14 cave road, 15 grass road, 16 mountain road
short general_types[260] = {1,1,0,0,0,1,1,1,1,1,
1,1,1,1,1,1,1,1,2,2,
2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,0,0,0,0, 
0,0,0,0,0,0,0,0,0,0,// 50
0,3,3,3,3,3,3,5,5,5,
6,6,7,7,1,1,8,9,10,11,
11,11,12,13,13,9,9,9,1,1,
1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,// 100
1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,// 150
1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,
1,0,1,1,1,1,1,1,1,0,
0,0,0,0,0,0,0,0,0,0,
0,0,14,15,16,0,0,1,1,1,// 200
1,0,2,1,1,1,0,1,1,1,
1,1,0,0,0,0,1,0,1,1,
1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1};// 250
short ter_base[17] = {2,0,36,50,71, 0,0,0,0,2, 2,2,2,0, 0,2,36};	
//short ground_type[14] = {2,0,36,50,71, 0,0,0,0,2,	2,2,2,0};

location special_ter_locs[15] =
	{ 
		location(11,10),location(11,14),location(10,20),location(11,26),location(9,30),
		location(15,19),location(23,19),location(19,29),location(20,11),location(28,16),
		location(28,24),location(27,19),location(27,29),location(15,28),location(19,19)
	};
unsigned char cave_pillar[4][4] = {{0,14,11,1},{14,19,20,11},{17,18,21,8},{1,17,8,0}};
unsigned char mntn_pillar[4][4] = {{37,29,27,36},{29,33,34,27},{31,32,35,25},{36,31,25,37}};
unsigned char surf_lake[4][4] = {{56,55,54,3},{57,50,61,54},{58,51,59,53},{3,4,58,52}};
unsigned char cave_lake[4][4] = {{93,96,71,71},{96,71,71,71},{71,71,71,96},{71,71,71,96}};
location stuff_ul;
short terrain_odds[17][10] = {{3,80,4,40,115,20,114,10,112,1}, 
							{1,50,93,25,94,5,98,10,95,1}, 
							{37,20,0,0,0,0,0,0,0,0}, 
							{64,3,63,1,0,0,0,0,0,0}, 
							{74,1,0,0,0,0,0,0,0,0}, 
							{84,700,97,30,98,20,92,4,95,1}, 
							{93,280,91,300,92,270,95,7,98,10}, 
							{1,800,93,600,94,10,92,10,95,4}, 
							{1,700,96,200,95,100,92,10,112,5}, 
							{3,600,87,90,110,20,114,6,113,2}, 
							{3,200,4,400,111,250,0,0,0,0}, 
							{3,200,4,300,112,50,113,60,114,100}, 
							{3,100,4,250,115,120,114,30,112,2}, 
							{1,25,84,15,98,300,97,280,0,0},
                            {1,50,93,25,94,5,98,10,95,1},
                            {3,80,4,40,115,20,114,10,112,1},
                            {37,20,0,0,0,0,0,0,0,0}}; // ter then odds then ter then odds ...


	ter_type = scenario.ter_types[type].picture;
    if(ter_type == 401 || ter_type == 402)
        ter_type = 4;
	else if (ter_type > 260)
		ter_type = 1;
		else ter_type = general_types[ter_type];
	
	for (i = 0; i < 48; i++)
		for (j = 0; j < 48; j++) {
			c_town.explored[i][j] = 0;
			misc_i[i][j] = 0;
			sfx[i][j] = 0;
			if ((j <= 8) || (j >= 35) || (i <= 8) || (i >= 35))
				t_d.terrain[i][j] = 90;
				else t_d.terrain[i][j] = ter_base[ter_type];
			}
	for (i = 0; i < 48; i++)
		for (j = 0; j < 48; j++) 
			for (k = 0; k < 5; k++)
				if ((t_d.terrain[i][j] != 90) && (get_ran(1,1,1000) < terrain_odds[ter_type][k * 2 + 1]))
					t_d.terrain[i][j] = terrain_odds[ter_type][k * 2];
	
	t_d.terrain[0][0] = ter_base[ter_type];
	
	if (ter_type == 3) {
		t_d.terrain[0][0] = 83;
		for (i = 15; i < 26; i++)
			for (j = 9; j < 35; j++)
				t_d.terrain[i][j] = 83;
		}
		
	if(ter_type == 4){
    	t_d.terrain[0][0] = 82;
	   for (i = 15; i < 26; i++)
	       for (j = 9; j < 35; j++)
		      t_d.terrain[i][j] = 82;
        }
    
	if (ter_type == 14 || ter_type == 15 || ter_type == 16) {
		t_d.terrain[0][0] = 82;
		for (i = 19; i < 23; i++)
			for (j = 9; j < 35; j++)
				t_d.terrain[i][j] = 82;
		}


	// Now place special lakes, etc.
	if (ter_type == 2)
		for (i = 0; i < 15; i++)
			if (get_ran(1,0,5) == 1) {
				stuff_ul = special_ter_locs[i];
				for (j = 0; j < 4; j++)
					for (k = 0; k < 4; k++)
						t_d.terrain[stuff_ul.x + j][stuff_ul.y + k] = mntn_pillar[k][j];
				}
	if (t_d.terrain[0][0] == 0)
		for (i = 0; i < 15; i++)
			if (get_ran(1,0,25) == 1) {
				stuff_ul = special_ter_locs[i];
				for (j = 0; j < 4; j++)
					for (k = 0; k < 4; k++)
						t_d.terrain[stuff_ul.x + j][stuff_ul.y + k] = cave_pillar[k][j];
				}
	if (t_d.terrain[0][0] == 0)
		for (i = 0; i < 15; i++)
			if (get_ran(1,0,40) == 1) {
				stuff_ul = special_ter_locs[i];
				for (j = 0; j < 4; j++)
					for (k = 0; k < 4; k++)
						t_d.terrain[stuff_ul.x + j][stuff_ul.y + k] = cave_lake[k][j];
				}
	if (t_d.terrain[0][0] == 2)
		for (i = 0; i < 15; i++)
			if (get_ran(1,0,40) == 1) {
				stuff_ul = special_ter_locs[i];
				for (j = 0; j < 4; j++)
					for (k = 0; k < 4; k++)
						t_d.terrain[stuff_ul.x + j][stuff_ul.y + k] = surf_lake[k][j];
				}


	if (ter_base[ter_type] == 0) {
	for (i = 0; i < num_walls; i++) {
		r1 = get_ran(1,0,3);
		for (j = 9; j < 35; j++)
			switch (r1) {
				case 0:
					t_d.terrain[j][8] = 6;
					break;
				case 1:
					t_d.terrain[8][j] = 9;
					break;
				case 2:
					t_d.terrain[j][35] = 12;
					break;
				case 3:
					t_d.terrain[32][j] = 15;
					break;
			}			
		}
	if ((t_d.terrain[17][8] == 6) && (t_d.terrain[8][20] == 9))
		t_d.terrain[8][8] = 21;
	if ((t_d.terrain[32][20] == 15) && (t_d.terrain[17][35] == 12))
		t_d.terrain[32][35] = 19;	
	if ((t_d.terrain[17][8] == 6) && (t_d.terrain[32][20] == 15))
		t_d.terrain[32][8] = 32;
	if ((t_d.terrain[8][20] == 9) && (t_d.terrain[17][35] == 12))
		t_d.terrain[8][35] = 20;	
	}
	if (ter_base[ter_type] == 36) {
	for (i = 0; i < num_walls; i++) {
		r1 = get_ran(1,0,3);
		for (j = 9; j < 35; j++)
			switch (r1) {
				case 0:
					t_d.terrain[j][8] = 24;
					break;
				case 1:
					t_d.terrain[8][j] = 26;
					break;
				case 2:
					t_d.terrain[j][35] = 28;
					break;
				case 3:
					t_d.terrain[32][j] = 30;
					break;
			}			
		}
	if ((t_d.terrain[17][8] == 6) && (t_d.terrain[8][20] == 9))
		t_d.terrain[8][8] = 35;
	if ((t_d.terrain[32][20] == 15) && (t_d.terrain[17][35] == 12))
		t_d.terrain[32][35] = 33;	
	if ((t_d.terrain[17][8] == 6) && (t_d.terrain[32][20] == 15))
		t_d.terrain[32][8] = 32;
	if ((t_d.terrain[8][20] == 9) && (t_d.terrain[17][35] == 12))
		t_d.terrain[8][35] = 34;	
	}
	
	for (i = 0; i < 48; i++)
		for (j = 0; j < 48; j++) 
			combat_terrain[i][j] = t_d.terrain[i][j];
			
	make_town_trim(1);
}


/*void elim_monst(unsigned char which,short spec_a,short spec_b)
{
	short i;
	
	if (party.stuff_done[spec_a][spec_b] > 0) {
		for (i = 0; i < T_M; i++)
			if (c_town.monst.dudes[i].number == which) {
				c_town.monst.dudes[i].active = 0;
				}
		}
}*/



void dump_gold(short print_mes)
//short print_mes; // 0 - no 1 - yes
{
	// Mildly kludgy gold check
	if (party.gold > 30000) {
		party.gold = 30000;
		if (print_mes == 1) {
			put_pc_screen();
			add_string_to_buf("Excess gold dropped.            ");
			print_buf();
			}
		}
	if (party.food > 25000) {
		party.food = 25000;
		if (print_mes == 1) {
			put_pc_screen();
			add_string_to_buf("Excess food dropped.            ");
			print_buf();
			}
		}
}

void erase_specials()
{
	location where;
	short k,sd1,sd2;

	special_node_type sn;
	
	if ((is_combat()) && (which_combat_type == 0))
		return;
	if ((is_town() == FALSE) && (is_combat() == FALSE))
		return;
	for (k = 0; k < 50; k++) {
		//GK if (c_town.town.spec_id[k] >= 0) 
		{
			sn = c_town.town.specials[c_town.town.spec_id[k]];
			sd1 = sn.sd1; sd2 = sn.sd2;
			if ((sd_legit(sd1,sd2) == TRUE) && (PSD[sd1][sd2] == 250)) {
				where = c_town.town.special_locs[k];
				if ((where.x != 100) && ((where.x > town_size[town_type]) || (where.y > town_size[town_type])
				 || (where.x < 0) || (where.y < 0))) {
					MessageBeep(MB_OK);
					add_string_to_buf("Town corrupt. Problem fixed.");
					print_nums(where.x,where.y,k);
					c_town.town.special_locs[k].x = 0;
					}
				
				if (where.x != 100) {
					switch (scenario.ter_types[t_d.terrain[where.x][where.y]].picture) {
						case 207: t_d.terrain[where.x][where.y] = 0; break;
						case 208: t_d.terrain[where.x][where.y] = 170; break;
						case 209: t_d.terrain[where.x][where.y] = 210; break;
						case 210: t_d.terrain[where.x][where.y] = 217; break;
						case 211: t_d.terrain[where.x][where.y] = 2; break;
						case 212: t_d.terrain[where.x][where.y] = 36; break;
						}
					take_special(where.x,where.y);
					}
				}
			

				}
		}
}

void erase_out_specials()
{
	short i,j,k,l,m,out_num;

	special_node_type sn;
	short sd1,sd2;
	shortloc where;
	
	for (k = 0; k < 2; k++)
	{
		for (l = 0; l < 2; l++)
		{
			if (quadrant_legal(k,l) == TRUE)
			{
				for (m = 0; m < 8; m++)
				{
					if ((outdoors[k][l].exit_dests[m] >= 0) &&
							(outdoors[k][l].exit_locs[m].x == minmax(0,47,(int)outdoors[k][l].exit_locs[m].x)) &&
							(outdoors[k][l].exit_locs[m].y == minmax(0,47,(int)outdoors[k][l].exit_locs[m].y)))
					{
						if (party.can_find_town[outdoors[k][l].exit_dests[m]] == 0)
						{
						out[48 * k + outdoors[k][l].exit_locs[m].x][48 * l + outdoors[k][l].exit_locs[m].y] = 
							scenario.ter_types[outdoors[k][l].terrain[outdoors[k][l].exit_locs[m].x][outdoors[k][l].exit_locs[m].y]].flag1;
						}
						else if (party.can_find_town[outdoors[k][l].exit_dests[m]] > 0)
						{
						out[48 * k + outdoors[k][l].exit_locs[m].x][48 * l + outdoors[k][l].exit_locs[m].y] = 
							outdoors[k][l].terrain[outdoors[k][l].exit_locs[m].x][outdoors[k][l].exit_locs[m].y];
						}
					}
				}
			}
		}
	}
	
	for (i = 0; i < 2; i++)
		for (j = 0; j < 2; j++)
			if (quadrant_legal(i,j) == TRUE) {
			for (k = 0; k < 18; k++) 
				//GK if (outdoors[i][j].special_id[k] >= 0)
				{
				out_num = scenario.out_width * (party.outdoor_corner.y + j) + party.outdoor_corner.x + i;

				sn = outdoors[i][j].specials[outdoors[i][j].special_id[k]];
				sd1 = sn.sd1; sd2 = sn.sd2;
				if ((sd_legit(sd1,sd2) == TRUE) && (PSD[sd1][sd2] == 250)) {
					where.x = outdoors[i][j].special_locs[k].x;
					where.y = outdoors[i][j].special_locs[k].y;
					if (where.x != 100) {
						if ((where.x > 48) || (where.y > 48)
						 || (where.x < 0) || (where.y < 0)) {
							MessageBeep(MB_OK);
							add_string_to_buf("Outdoor section corrupt. Problem fixed.");
							//GK outdoors[i][j].special_id[k] = -1;
							outdoors[i][j].special_id[k] = 0xFF;
							}
				
						switch (scenario.ter_types[outdoors[i][j].terrain[where.x][where.y]].picture) {
							case 207: out[48 * i + where.x][48 * j + where.y] = 0; break;
							case 208: out[48 * i + where.x][48 * j + where.y] = 170; break;
							case 209: out[48 * i + where.x][48 * j + where.y] = 210; break;
							case 210: out[48 * i + where.x][48 * j + where.y] = 217; break;
							case 211: out[48 * i + where.x][48 * j + where.y] = 2; break;
							case 212: out[48 * i + where.x][48 * j + where.y] = 36; break;
							}
						}
					}

			
				}
		}
}

void clear_map()
{
	RECT map_world_rect = {0,0,384,384};
	HBITMAP old_bmp;
	HDC hdc;
	HBRUSH oldb;
	HPEN oldp;

	hdc = CreateCompatibleDC(main_dc);
	old_bmp = (HBITMAP) SelectObject(hdc, map_gworld);

	oldp = (HPEN) SelectObject(hdc,GetStockObject(BLACK_PEN));
	oldb = (HBRUSH) SelectObject(hdc,GetStockObject(BLACK_BRUSH));
	Rectangle(hdc, map_world_rect.left,map_world_rect.top,map_world_rect.right,map_world_rect.bottom);
	SelectObject(hdc,oldp);
	SelectObject(hdc,oldb);
	SelectObject(hdc, old_bmp);
	DeleteDC(hdc);

	draw_map(modeless_dialogs[5],10);
}


void draw_map_rect (HWND the_dialog, short ul_x, short ul_y, short lr_x, short lr_y)            //to clean
//ul = upper left
//lr = lower right

{

	location map_adj;
	location where;
	location kludge;
	RECT area_to_put_on_map_rect;
	RECT draw_rect;
	RECT custom_from;
	RECT ter_temp_from,orig_draw_rect = {0,0,6,6};
	RECT redraw_rect = {0,0,48,48}; // RECTangle visible in view screen
	COLORREF map_colors[6] = {RGB(0,0,0),RGB(63,223,95),RGB(0,0,255),RGB(191,0,191),RGB(255,0,0),RGB(204,204,204)};
	HDC hdc = NULL;
	HBITMAP old_bmp;
	HBRUSH old_brush;
	HPEN old_pen;
	short i,pic,pic2;
	Boolean expl,expl2;
	short small_adj = 0;
	unsigned char what_ter,what_ter2;
	Boolean out_mode;

	// make map pens
	if (hbrush[0] == NULL) {
		for (i = 0; i < 6; i++) {
			hbrush[i] = CreateSolidBrush(map_colors[i]);
			hpen[i] = CreatePen(PS_SOLID,1,map_colors[i]);
			}
		}

	hdc = CreateCompatibleDC(main_dc);
	old_bmp = (HBITMAP) SelectObject(hdc, map_gworld);
	old_brush = (HBRUSH) SelectObject(hdc,map_brush[0]);
	old_pen = (HPEN) SelectObject(hdc,GetStockObject(NULL_PEN));
	

	if (is_out()) { // for outside map, adjust for where in outdoors is being mapped	
	if (party.i_w_c.x == 1)
		map_adj.x += 48;
	if  (party.i_w_c.y == 1)
		map_adj.y += 48;
		}		

	// Now, if shopping or talking, just don't touch anything.
	if ((overall_mode == 21) || (overall_mode == 20))
		redraw_rect.right = -1;
	
	if ((is_out()) ||
		((is_combat()) && (which_combat_type == 0)) ||
		((overall_mode == 20) && (store_pre_talk_mode == 0)) ||
		((overall_mode == 21) && (store_pre_shop_mode == 0)))
		out_mode = TRUE;
		else out_mode = FALSE;

	area_to_put_on_map_rect = redraw_rect;
	
		for (where.x= ul_x; where.x <= lr_x; where.x++)
			for (where.y= ul_y; where.y <= lr_y; where.y++){
					draw_rect = orig_draw_rect;
					OffsetRect(&draw_rect,6 * where.x + small_adj, 6 * where.y + small_adj);

					if (out_mode == TRUE)
						what_ter = out[where.x + 48 * party.i_w_c.x][where.y + 48 * party.i_w_c.y];
						else what_ter = t_d.terrain[where.x][where.y];
					
					ter_temp_from = orig_draw_rect;
					
					if (out_mode == TRUE)
						expl = out_e[where.x + 48 * party.i_w_c.x][where.y + 48 * party.i_w_c.y];
						else expl = is_explored(where.x,where.y);
						
					if (expl != 0) {
						map_graphic_placed[where.x / 8][where.y] = 
							map_graphic_placed[where.x / 8][where.y] | (unsigned char)(s_pow(2,where.x % 8));
						pic = scenario.ter_types[what_ter].picture;
						if (pic >= 1000) {

							if (spec_scen_g != NULL) {

								pic = pic % 1000;
								custom_from = coord_to_rect(pic % 10, pic / 10);
								OffsetRect(&custom_from,-13,-13);
								SelectObject(hdc,old_bmp);
								rect_draw_some_item(spec_scen_g,custom_from,map_gworld,draw_rect,0,0);
 								SelectObject(hdc,map_gworld);
								}
							}
						else switch ((pic >= 400) ? anim_map_pats[pic - 400] : map_pats[pic]) {
							case 0: case 10: case 11:
								if (terrain_pic[what_ter] < 400)
									OffsetRect(&ter_temp_from,
										6 * (terrain_pic[what_ter] % 10) + 312,6 * (terrain_pic[what_ter] / 10));
									else OffsetRect(&ter_temp_from,
										24 * ((terrain_pic[what_ter] - 400) / 5) + 312,6 * ((terrain_pic[what_ter] - 400) % 5) + 163);
								SelectObject(hdc,old_bmp);
								rect_draw_some_item(mixed_gworld,ter_temp_from,
									map_gworld,draw_rect,0,0);
								SelectObject(hdc,map_gworld);
								break;

							default:
								if (((pic >= 400) ? anim_map_pats[pic - 400] : map_pats[pic]) < 30) {
									// Try a little optimization
									if ((pic < 400) && (where.x < area_to_put_on_map_rect.right - 1)) {
										if (out_mode == TRUE)
											what_ter2 = out[(where.x + 1)+ 48 * party.i_w_c.x][where.y + 48 * party.i_w_c.y];
											else what_ter2 = t_d.terrain[where.x + 1][where.y];	
										if (out_mode == TRUE)
											expl2 = out_e[(where.x + 1) + 48 * party.i_w_c.x][where.y + 48 * party.i_w_c.y];
											else expl2 = is_explored(where.x + 1,where.y);									
										pic2 = scenario.ter_types[what_ter2].picture;
										if (pic2 < 400 && (map_pats[pic] == map_pats[pic2]) && (expl2 != 0)) {
											draw_rect.right += 6;
											map_graphic_placed[(where.x + 1)/ 8][where.y] = 
												map_graphic_placed[(where.x + 1)/ 8][where.y] | (unsigned char)(s_pow(2,(where.x + 1)% 8));
											}
										}
									SelectObject(hdc,map_brush[((pic >= 400) ? anim_map_pats[pic - 400] : map_pats[pic]) - 1]);
									Rectangle(hdc,draw_rect.left, draw_rect.top,draw_rect.right + 1,draw_rect.bottom + 1);
									break;
									}
/*								OffsetRect(&ter_temp_from,
									312 + 24 * ((map_pats[what_ter] - 30) / 5),
									138 + 6 * ((map_pats[what_ter] - 30) % 5));
								SelectObject(hdc,old_bmp);
								rect_draw_some_item(mixed_gworld,ter_temp_from,
									map_gworld,draw_rect,0,0);
								SelectObject(hdc,map_gworld);*/
								break;
							}
						}
				

		}
	
	SelectObject(hdc,old_brush);
	SelectObject(hdc,old_pen);
	SelectObject(hdc,old_bmp);
	DeleteDC(hdc);

}

void draw_map (HWND the_dialog, short the_item)
//short the_item; // Being sneaky - if this gets value of 5, this is not a full restore -
				// just update near party, if it gets 11, blank out middle and leave
				// No redrawing in gworld
				// If a 10, do a regular full restore
				// Also, can get a 5 even when the window is not up, so have to make
				// sure dialog exists before accessing it.
{
	RECT map_world_rect = {0,0,321,321};
	RECT whole_map_win_rect = {0,0,400,400};

	location map_adj;
	location where;
	location kludge;
	RECT area_to_draw_from,area_to_draw_on = {47,29,287,269};
	RECT area_to_put_on_map_rect;
	RECT draw_rect;
	RECT custom_from;
	RECT ter_temp_from,dlogpicrect = {6,6,42,42},orig_draw_rect = {0,0,6,6};
	RECT view_rect= {0,0,48,48},tiny_rect = {0,0,32,32},
		redraw_rect = {0,0,48,48},big_rect = {0,0,64,64}; // RECTangle visible in view screen
	COLORREF map_colors[6] = {RGB(0,0,0),RGB(63,223,95),RGB(0,0,255),RGB(191,0,191),RGB(255,0,0),RGB(204,204,204)};
	HDC hdc = NULL,hdc2;
	HBITMAP old_bmp;
	HBRUSH old_brush;
	HPEN old_pen;
	short i,j,pic,pic2;
	Boolean draw_surroundings = FALSE,expl,expl2;
	Boolean draw_pcs = TRUE;
	short total_size = 48; // if full redraw, use this to figure out everything
	short small_adj = 0;
	unsigned char what_ter,what_ter2;
	Boolean out_mode;
//	UINT c;

if(c_town.town.defy_mapping == 1 ){//map not available
if (modeless_exists[5] == TRUE){
//    RECT text_rect = {30,265,230,275}; // legacy text position (lower left corner)
    RECT text_rect = {0,0,300,290};
	hdc2 = GetDC(the_dialog);
	paint_pattern((HBITMAP) hdc2,2,map_world_rect,0);
	SetBkMode(hdc2,TRANSPARENT);
   	SelectObject(hdc2,bold_font);
    SetTextColor(hdc2,RGB(255,255,255));
	char_win_draw_string(hdc2,text_rect,"This place defies mapping.",1,10);
	GetClientRect(GetDlgItem(the_dialog,1),&draw_rect);
	InvalidateRect(GetDlgItem(the_dialog,1),&draw_rect,FALSE);
    fry_dc(the_dialog,hdc2);
}
    return;
}
    if (the_item == 4) {
		draw_surroundings = TRUE;
		the_item = 5;
		}
	if (kludge_force_full_refresh == TRUE)
		draw_surroundings = TRUE;
	if ((modeless_exists[5] == FALSE) && (the_item == 5) && (need_map_full_refresh == TRUE))
		return;
	if ((modeless_exists[5] == FALSE) && (the_item == 10)) {
		need_map_full_refresh = TRUE;
		return;
		}
	if ((modeless_exists[5] == TRUE) && (the_item != 11) && (need_map_full_refresh == TRUE)) {
		need_map_full_refresh = FALSE;
		the_item = 10;
		}

   OffsetRect(&area_to_draw_on,0,-23);

	if (the_item == 10) {
		for (i = 0; i < 8; i++)
			for (j = 0; j < 64; j++)
				map_graphic_placed[i][j] = 0;
		}

	town_map_adj.x = 0;
	town_map_adj.y = 0;
	// view rect is rect that is visible, redraw rect is area to redraw now
	// area_to_draw_from is final draw from rect
	// area_to_draw_on is final draw to rect
	if ((is_out()) || ((is_combat()) && (which_combat_type == 0)) ||
		((overall_mode == 20) && (store_pre_talk_mode == 0)) ||
		((overall_mode == 21) && (store_pre_shop_mode == 0))) {
		view_rect.left = minmax(0,8,party.loc_in_sec.x - 20);
		view_rect.right = view_rect.left + 40;
		view_rect.top = minmax(0,8,party.loc_in_sec.y - 20);
		view_rect.bottom = view_rect.top + 40;
		redraw_rect = view_rect;
		}
		else {
			switch (town_type) {
				case 0:
					view_rect.left = minmax(0,24,c_town.p_loc.x - 20);
					view_rect.right = view_rect.left + 40;
					view_rect.top = minmax(0,24,c_town.p_loc.y - 20);
					view_rect.bottom = view_rect.top + 40;
					if (the_item == 5)
						redraw_rect = view_rect;
						else redraw_rect = big_rect;
					total_size = 64;
					break;
				case 1:
					view_rect.left = minmax(0,8,c_town.p_loc.x - 20);
					view_rect.right = view_rect.left + 40;
					view_rect.top = minmax(0,8,c_town.p_loc.y - 20);
					view_rect.bottom = view_rect.top + 40;
					redraw_rect = view_rect;
					break;
				case 2:
					view_rect = tiny_rect;
					redraw_rect = view_rect;
					total_size = 32;
					break;
				}
			}
	if ((is_out()) || ((is_combat()) && (which_combat_type == 0)) ||
		((overall_mode == 20) && (store_pre_talk_mode == 0)) ||
		((overall_mode == 21) && (store_pre_shop_mode == 0)) ||
		(((is_town()) || (is_combat())) && (town_type != 2))) {
			area_to_draw_from = view_rect;	
			area_to_draw_from.left *= 6;
			area_to_draw_from.right *= 6;
			area_to_draw_from.top *= 6;
			area_to_draw_from.bottom *= 6;
			}
			else {
				area_to_draw_from = area_to_draw_on;
				OffsetRect(&area_to_draw_from,-1 * area_to_draw_from.left,-1 * area_to_draw_from.top);
				small_adj = 0;
				}
			
	if (is_combat())
		draw_pcs = FALSE;

	// make map pens
	if (hbrush[0] == NULL) {
		for (i = 0; i < 6; i++) {
			hbrush[i] = CreateSolidBrush(map_colors[i]);
			hpen[i] = CreatePen(PS_SOLID,1,map_colors[i]);
			}
		}

	hdc = CreateCompatibleDC(main_dc);
	old_bmp = (HBITMAP) SelectObject(hdc, map_gworld);
	old_brush = (HBRUSH) SelectObject(hdc,map_brush[0]);
	old_pen = (HPEN) SelectObject(hdc,GetStockObject(NULL_PEN));
	
	if (the_item == 11) {
			SelectObject(hdc,GetStockObject(WHITE_BRUSH));
			Rectangle(hdc, map_world_rect.left,map_world_rect.top,map_world_rect.right,map_world_rect.bottom);
			draw_pcs = FALSE;
		}
	else {
	if (modeless_exists[5] == TRUE) {
		SetDlgItemText(the_dialog,3,"");
		}

	if (is_out()) { // for outside map, adjust for where in outdoors is being mapped	
	if (party.i_w_c.x == 1)
		map_adj.x += 48;
	if  (party.i_w_c.y == 1)
		map_adj.y += 48;
		}		

	// Now, if doing just partial restore, crop redraw_rect to save time.
	if (the_item == 5) {
		if ((is_out())  || ((is_combat()) && (which_combat_type == 0)) ||
		((overall_mode == 20) && (store_pre_talk_mode == 0)) ||
		((overall_mode == 21) && (store_pre_shop_mode == 0)))
			kludge = party.p_loc.toLocal();
		else if (is_combat())
			kludge = pc_pos[current_pc];
		else kludge = c_town.p_loc;
		redraw_rect.left = max(0,kludge.x - 4);
		redraw_rect.right = min(view_rect.right,kludge.x + 5);
		redraw_rect.top = max(0,kludge.y - 4);
		redraw_rect.bottom = min(view_rect.bottom,kludge.y + 5);
		} 

	// Now, if shopping or talking, just don't touch anything.
	if ((overall_mode == 21) || (overall_mode == 20))
		redraw_rect.right = -1;
	
	if ((is_out()) ||
		((is_combat()) && (which_combat_type == 0)) ||
		((overall_mode == 20) && (store_pre_talk_mode == 0)) ||
		((overall_mode == 21) && (store_pre_shop_mode == 0)))
		out_mode = TRUE;
		else out_mode = FALSE;

	area_to_put_on_map_rect = redraw_rect;
	if (the_item == 10) {
		area_to_put_on_map_rect.top = area_to_put_on_map_rect.left = 0; 
		area_to_put_on_map_rect.right = area_to_put_on_map_rect.bottom = total_size; 
		}

		for (where.x= area_to_put_on_map_rect.left; where.x < area_to_put_on_map_rect.right; where.x++)
			for (where.y= area_to_put_on_map_rect.top; where.y < area_to_put_on_map_rect.bottom; where.y++)
			  if ((map_graphic_placed[where.x / 8][where.y] & (unsigned char)(s_pow(2,where.x % 8))) == 0)
				{
					draw_rect = orig_draw_rect;
					OffsetRect(&draw_rect,6 * where.x + small_adj, 6 * where.y + small_adj);

					if (out_mode == TRUE)
						what_ter = out[where.x + 48 * party.i_w_c.x][where.y + 48 * party.i_w_c.y];
						else what_ter = t_d.terrain[where.x][where.y];
					
					ter_temp_from = orig_draw_rect;
					
					if (out_mode == TRUE)
						expl = out_e[where.x + 48 * party.i_w_c.x][where.y + 48 * party.i_w_c.y];
						else expl = is_explored(where.x,where.y);
						
					if (expl != 0) {
						map_graphic_placed[where.x / 8][where.y] = 
							map_graphic_placed[where.x / 8][where.y] | (unsigned char)(s_pow(2,where.x % 8));
						pic = scenario.ter_types[what_ter].picture;
						if (pic >= 1000) {

							if (spec_scen_g != NULL) {

								pic = pic % 1000;
								custom_from = coord_to_rect(pic % 10, pic / 10);
								OffsetRect(&custom_from,-13,-13);
								SelectObject(hdc,old_bmp);
								rect_draw_some_item(spec_scen_g,custom_from,map_gworld,draw_rect,0,0);
 								SelectObject(hdc,map_gworld);
								}
							}
						else switch ((pic >= 400) ? anim_map_pats[pic - 400] : map_pats[pic]) {
							case 0: case 10: case 11:
                            	if (terrain_pic[what_ter] < 400)
									OffsetRect(&ter_temp_from,
										6 * (terrain_pic[what_ter] % 10) + 312,6 * (terrain_pic[what_ter] / 10));
									else OffsetRect(&ter_temp_from,
										24 * ((terrain_pic[what_ter] - 400) / 5) + 312,6 * ((terrain_pic[what_ter] - 400) % 5) + 163);
								SelectObject(hdc,old_bmp);
								rect_draw_some_item(mixed_gworld,ter_temp_from,
									map_gworld,draw_rect,0,0);
								SelectObject(hdc,map_gworld);
								break;

							default:
//								if (((pic >= 400) ? anim_map_pats[pic - 400] : map_pats[pic]) < 30) {//always true ?!?
									if ((pic < 400) && (where.x < area_to_put_on_map_rect.right - 1)) {// Try a little optimization
										if (out_mode == TRUE)
											what_ter2 = out[(where.x + 1)+ 48 * party.i_w_c.x][where.y + 48 * party.i_w_c.y];
											else what_ter2 = t_d.terrain[where.x + 1][where.y];	
										if (out_mode == TRUE)
											expl2 = out_e[(where.x + 1) + 48 * party.i_w_c.x][where.y + 48 * party.i_w_c.y];
											else expl2 = is_explored(where.x + 1,where.y);									
										pic2 = scenario.ter_types[what_ter2].picture;
										if (pic2 < 400 && (map_pats[pic] == map_pats[pic2]) && (expl2 != 0)) {
											draw_rect.right += 6;
											map_graphic_placed[(where.x + 1)/ 8][where.y] = 
												map_graphic_placed[(where.x + 1)/ 8][where.y] | (unsigned char)(s_pow(2,(where.x + 1)% 8));
											}
										}
									SelectObject(hdc,map_brush[((pic >= 400) ? anim_map_pats[pic - 400] : map_pats[pic]) - 1]);
									Rectangle(hdc,draw_rect.left, draw_rect.top,draw_rect.right + 1,draw_rect.bottom + 1);
									break;
//									}
/*								OffsetRect(&ter_temp_from,
									312 + 24 * ((map_pats[what_ter] - 30) / 5),
									138 + 6 * ((map_pats[what_ter] - 30) % 5));//138 ??? <= nevermind cause never called ...
								SelectObject(hdc,old_bmp);
								rect_draw_some_item(mixed_gworld,ter_temp_from,
									map_gworld,draw_rect,0,0);
								SelectObject(hdc,map_gworld);*/
								break;
							}
						}
				}

		}
	
	SelectObject(hdc,old_brush);
	SelectObject(hdc,old_pen);
	SelectObject(hdc,old_bmp);
	DeleteDC(hdc);

  	// Now place terrain map gworld
	if (modeless_exists[5] == TRUE) {

		// graphics goes here
		hdc2 = GetDC(the_dialog);
		if ((draw_surroundings == TRUE) || (the_item != 5)) { // redraw much stuff
			paint_pattern((HBITMAP) hdc2,2,whole_map_win_rect,0);
			SetBkMode(hdc2,TRANSPARENT);
			SelectObject(hdc2,small_bold_font);
			GetClientRect(GetDlgItem(the_dialog,1),&draw_rect);
			InvalidateRect(GetDlgItem(the_dialog,1),&draw_rect,FALSE);
			}

        rect_draw_some_item(map_gworld,area_to_draw_from,(HBITMAP) hdc2,area_to_draw_on,0,2);
		}

	// Now place PCs and monsters
	if ((draw_pcs == TRUE) && (modeless_exists[5] == TRUE)) {
		if ((is_town()) && (party.stuff_done[305][2] > 0))
			for (i = 0; i < T_M; i++) 
				if (c_town.monst.dudes[i].active > 0) {
					where = c_town.monst.dudes[i].m_loc;
					if ((is_explored(where.x,where.y)) && 
						((where.x >= view_rect.left) && (where.x <= view_rect.right) 
						&& (where.y >= view_rect.top) && (where.x <= view_rect.bottom))){

						draw_rect.left = area_to_draw_on.left + 6 * (where.x - view_rect.left);
						draw_rect.top = area_to_draw_on.top + 6 * (where.y - view_rect.top);
						draw_rect.right = draw_rect.left + 6;
						draw_rect.bottom = draw_rect.top + 6;

						map_graphic_placed[where.x / 8][where.y] = 
							map_graphic_placed[where.x / 8][where.y] & ~((unsigned char)(s_pow(2,where.x % 8)));
							SelectObject(hdc2,hpen[0]);
							SelectObject(hdc2,hbrush[0]);
							Rectangle(hdc2, draw_rect.left,draw_rect.top,draw_rect.right,draw_rect.bottom);
							SelectObject(hdc2,hpen[4]);
							SelectObject(hdc2,hbrush[5]);
							Ellipse(hdc2, draw_rect.left,draw_rect.top,draw_rect.right,draw_rect.bottom);
						}
				}

		if ((overall_mode != 21) && (overall_mode != 20)) {
			where = (is_town()) ? c_town.p_loc : party.p_loc.toLocal();

					draw_rect.left = area_to_draw_on.left + 6 * (where.x - view_rect.left);
					draw_rect.top = area_to_draw_on.top + 6 * (where.y - view_rect.top);
					draw_rect.right = draw_rect.left + 6;
					draw_rect.bottom = draw_rect.top + 6;
					if ((where.x >= 0) && (where.x < 64) &&
						(where.y >= 0) && (where.y < 64)) {
							map_graphic_placed[where.x / 8][where.y] =  /* Crash! vvv */
								map_graphic_placed[where.x / 8][where.y] & ~((unsigned char)(s_pow(2,where.x % 8)));

							SelectObject(hdc2,hpen[0]);
							SelectObject(hdc2,hbrush[0]);
							Rectangle(hdc2, draw_rect.left,draw_rect.top,draw_rect.right,draw_rect.bottom);
							SelectObject(hdc2,hpen[3]);
							SelectObject(hdc2,hbrush[3]);
							Ellipse(hdc2, draw_rect.left,draw_rect.top,draw_rect.right,draw_rect.bottom);
						  }
			}	
		}
	
	// Now exit gracefully

	if (modeless_exists[5] == TRUE) {

		// graphics goes here
   		fry_dc(the_dialog,hdc2);
   		if ((draw_surroundings == TRUE) || (the_item != 5)) { // redraw much stuff
			draw_dialog_graphic(the_dialog, dlogpicrect,
				721, FALSE,0); // draw the icon map graphic
			}
		}
}

BOOL CALLBACK map_dialog_proc(HWND hDlg, UINT message, WPARAM wParam, LPARAM)
{
	switch (message) {
		case WM_INITDIALOG:
			if (store_map_window_rect.right > 0)
				MoveWindow(hDlg,store_map_window_rect.left,store_map_window_rect.top,
					store_map_window_rect.right - store_map_window_rect.left,
					store_map_window_rect.bottom - store_map_window_rect.top,FALSE);
				else {
					GetWindowRect(hDlg,&store_map_window_rect);
					MoveWindow(hDlg,294 + ulx, 47 + uly,
						store_map_window_rect.right - store_map_window_rect.left,
						store_map_window_rect.bottom - store_map_window_rect.top,FALSE);

					}
			kludge_force_full_refresh = TRUE;
			draw_map(hDlg,10);
			kludge_force_full_refresh = FALSE;
			SetFocus(mainPtr);
			break;
		case WM_ERASEBKGND:
			return 1;

		case WM_PAINT:
			kludge_force_full_refresh = TRUE;
			draw_map(hDlg,5);
			kludge_force_full_refresh = FALSE;
			return FALSE;

		case WM_KEYDOWN:
			if (wParam != VK_RETURN)
				return 0;
		case WM_COMMAND:
			modeless_exists[5] = FALSE;
			GetWindowRect(hDlg,&store_map_window_rect);
			DestroyWindow(hDlg);
			return TRUE;
		case WM_DESTROY:
			modeless_exists[5] = FALSE;
			return 0;
		}
	return FALSE;
}

void display_map()
{
	if ( modeless_exists[5] == TRUE)
		return;
	
	modeless_exists[5] = TRUE;
		
	modeless_dialogs[5] = CreateDialog(store_hInstance,
		MAKEINTRESOURCE(1046),mainPtr,(DLGPROC) map_dialog_proc);
}

Boolean quadrant_legal(short i, short j) 
{
	if (party.outdoor_corner.x + i >= scenario.out_width) return FALSE;
	if (party.outdoor_corner.y + j >= scenario.out_height) return FALSE;
	if (party.outdoor_corner.x + i < 0)	return FALSE;
	if (party.outdoor_corner.y + j < 0)	return FALSE;
	return TRUE;
}
