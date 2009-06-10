#include "pc.h"				// party_record_type
#include "location.h"		// location
#include "text.h"			// add_string_to_buf
#include "blades.h"			// globals
#include "exlsound.h"		// play_sound
#include "combat.h"			// hit_chance
#include "items.h"			// place_item
#include "fields.h"			// make_sfx
#include "global.h"			// get_ran
#include "infodlgs.h"		// give_help
#include "party.h"			// adjust_spell_menu
#include "itemdata.h"		// return_dummy_item
#include "locutils.h"		// is_container
#include "globvar.h"

void pc_record_type::kill(short type)
{
	short i = 24;
	bool dummy, no_save = false;
	location item_loc;
	
	if (type >= 10)
	{
		type -= 10;
		no_save = true;
	}
	
	if (type != 4)
		i = hasAbilEquip(48); //check if has life saving items
    else
	    i = hasAbilEquip(49); //check if has protection vs petrification items
		
	short which_pc = getNum();

	if ((no_save == false) && (type != 0) && (skills[SKILL_LUCK] > 0) && 
		(get_ran(1,0,100) < hit_chance[skills[SKILL_LUCK]])) {
			add_string_to_buf("  But you luck out!          ");
			cur_health = 0;
			}
		else if ((i == 24) || (type == 0)) {
			if (combat_active_pc == which_pc)
				combat_active_pc = 6;
	
			for (i = 0; i < 24; i++)
				equip[i] = FALSE;

			item_loc = (overall_mode >= MODE_COMBAT) ? pc_pos[which_pc] : c_town.p_loc;
	
			if (type == 2)
				make_sfx(item_loc.x,item_loc.y,3);
				else if (type == 3)
					make_sfx(item_loc.x,item_loc.y,6);
		
			if (overall_mode != 0)	
				for (i = 0; i < 24; i++)
					if (items[i].variety != 0) {
						dummy = place_item(items[i],item_loc,TRUE);
						items[i].variety = 0;
						}
				if ((type == 2) || (type == 3))
					play_sound(21);
				if(type == 4){
                    play_sound(43);
                    sprintf ((char *) create_line, "  %s is turned to stone.                  ",(char *) name);
					add_string_to_buf((char *) create_line);
                    }
				main_status = type;
				pc_moves[which_pc] = 0;
			}
			else {
                if (type == 4) {
                  sprintf ((char *) create_line, "  %s is immuned to petrification.                  ",(char *) name);
                  add_string_to_buf((char*) create_line); //inform of what has happened
                                    }
                else{
				add_string_to_buf("  Life saved!              ");
				takeItem(i);
				heal(200);
               }
            }

        current_pc = first_active_pc();
	
	if (current_pc > 5) {
		for (i = 0; i < NUM_OF_PCS; i++)
			if (adven[i].status > 0)
				current_pc = i;
		}
		
	put_pc_screen();
	set_stat_window(current_pc);
}

bool pc_record_type::runTrap(short trap_type, short trap_level, short diff)
{
	short r1,skill,i,num_hits = 1,i_level;
	short trap_odds[30] = {5,30,35,42,48, 55,63,69,75,77,
							78,80,82,84,86, 88,90,92,94,96,98,99,99,99,99,99,99,99,99,99};

	num_hits += trap_level;
		
	if (trap_type == TRAP_RANDOM)
		trap_type = get_ran(1,1,4);
		
	/// ??? => no trap after all ;)
	if (trap_type == 6)
		return true;
		
	i = statAdj(1);
	
	if ((i_level = getProtLevel(42)) > 0)
		i += i_level / 2;
		
	skill = minmax(0, 20, skills[14] + skills[SKILL_LUCK] / 2 + 1 - c_town.difficulty + 2 * i);

	r1 = get_ran(1,0,100) + diff;
	// Nimble?
	if (traits[TRAIT_NIMBLE] == TRUE)	r1 -= 6;

	if (r1 < trap_odds[skill])
	{
		add_string_to_buf("  Trap disarmed.            ");
		return true;
	}
		else add_string_to_buf("  Disarm failed.          ");
	
	switch (trap_type)
	{
		case TRAP_BLADE:
			for (i = 0; i < num_hits; i++)
			{
				add_string_to_buf("  A knife flies out!              ");
				damage(get_ran(2 + c_town.difficulty / 14,1,10),DAMAGE_WEAPON,-1);
			}
			break;
			
		case TRAP_DART:
			add_string_to_buf("  A dart flies out.              ");
			poison((3 + c_town.difficulty / 14) + (trap_level * 2));
			break;
			
		case TRAP_GAS:
			add_string_to_buf("  Poison gas pours out.          ");
			adven.poison((2 + c_town.difficulty / 14) + trap_level * 2);
			break;
	
		case TRAP_EXPLOSION: 
			for (i = 0; i < num_hits; i++)
			{
				add_string_to_buf("  There is an explosion.        ");
				adven.damage(get_ran(3 + c_town.difficulty / 13,1,8), DAMAGE_FIRE);
			}
			break;
			
		case TRAP_SLEEP_RAY:
			add_string_to_buf("  A purple ray flies out.          ");
			sleep((200 + c_town.difficulty * 100) + (trap_level * 400),12,50);
			break;
			
		case TRAP_DRAIN_XP:
			add_string_to_buf("  You feel weak.            ");
			experience = max(0, experience - (40 + trap_level * 30));
			break;
		
		case TRAP_ALERT:
			add_string_to_buf("  An alarm goes off!!!            ");
			make_town_hostile();
			break;

		case TRAP_FLAMES: 
			add_string_to_buf("  Flames shoot from the walls.        ");
			adven.damage(get_ran(10 + trap_level * 5,1,8),DAMAGE_FIRE);
			break;
			
		case TRAP_DUMBFOUND: 
			add_string_to_buf("  You feel disoriented.        ");
			adven.dumbfound(2 + trap_level * 2);
			break;
			
		case TRAP_DISEASE:
			add_string_to_buf("  You prick your finger. ");
			disease((3 + c_town.difficulty / 14) + (trap_level * 2));
			break;
			
		case TRAP_DISEASE_ALL:
			add_string_to_buf("  A foul substance sprays out.");
			adven.disease((2 + c_town.difficulty / 14) + (trap_level * 2));
			break;
			
		default:
			add_string_to_buf("  (ERROR: Unknown type of trap!)");
			break;
	}
	
	put_pc_screen();
	put_item_screen(stat_window,0);
	return true;
}

short pc_record_type::statAdj(short which)
{
	short tr;
	
	tr = skill_bonus[skills[which]];
	
	if (which == 2) {
		if (traits[1] == TRUE) tr++;
		if (hasAbilEquip(40) < 24) tr++;
		}
		
	if (which == 0)
		if (traits[8] == TRUE) tr++;
        if (hasAbilEquip(38) < 24) 
            tr++;
            
    if (which == 1) {
        if (hasAbilEquip(39) < 24) 
            tr++;
    }
		
	return tr;
}

void pc_record_type::giveXP(short amt)
{
	short adjust,add_hp;
	short xp_percent[30] = {150,120,100,90,80,70,60,50,50,50,
								45,40,40,40,40,35,30,25,23,20,
								15,15,15,15,15,15,15,15,15,15};	
								
	if (!isAlive())	return;
	
	if (level > 49)
	{
		level = 50;
		return;
	}
	
	if (amt > 200) { // debug
		MessageBeep(MB_OK);
		ASB("Oops! Too much xp!");
		ASB("Report this!");
		return;
		}
		
	if (amt < 0) { // debug
		MessageBeep(MB_OK);
		ASB("Oops! Negative xp!");
		ASB("Report this!");
		return;
		}

	if (level >= 40) adjust = 15;
	else adjust = xp_percent[level / 2];

	if ((amt > 0) && (level > 7))
	{
		if (get_ran(1,0,100) < xp_percent[level / 2]) amt--;
	}
	
	if (amt <= 0) return;

	experience += (max(((amt * adjust) / 100), 0) * 100) / 100;
	
	party.total_xp_gained += (max(((amt * adjust) / 100), 0) * 100) / 100;
	
	if (experience < 0) {
		MessageBeep(MB_OK);
		ASB("Oops! Xp became negative somehow!");
		ASB("Report this!");
		experience = level * (get_tnl(this)) - 1;
		return;
		}
		
	if (experience > 15000)
	{
		experience = 15000;
		return;
	}
	
	while (experience >= (level * (get_tnl(this))))
	{
		play_sound(7);
		level++;
		
		sprintf ((char *) c_line, "  %s is level %d!  ",(char *) name, level);
		add_string_to_buf((char *) c_line);		
		skill_pts += (level < 20) ? 5 : 4;
		add_hp = (level < 26) ? get_ran(1,2,6) + skill_bonus[skills[0]]
		   : max (skill_bonus[skills[0]],0);
		   
		if (add_hp < 0) add_hp = 0;
		max_health += add_hp;
		
		if (max_health > 250) max_health = 250;
		cur_health += add_hp;
		
		if (cur_health > 250) cur_health = 250;
		
		put_pc_screen();		
	}
}

void pc_record_type::drainXP(short how_much)
{
	if (!isAlive()) return;

	experience = max(experience - how_much, 0);
	sprintf ((char *) c_line, "  %s drained.",(char *) name);
	add_string_to_buf((char *) c_line);
}

void pc_record_type::restoreSP(short amt)
{
	if (!isAlive()) return;
	if (cur_sp > max_sp) return;
	cur_sp += amt;
	if (cur_sp > max_sp) cur_sp = max_sp;
}

void pc_record_type::cure(short amt)
{
	if (!isAlive()) return;

	if (status[STATUS_POISON] <= amt)
		status[STATUS_POISON] = 0;
	else
		status[STATUS_POISON] -= amt;
	
	one_sound(51);
}

void pc_record_type::curse(short how_much)
{
	if (!isAlive()) return;
	
	status[STATUS_BLESS] = max(status[STATUS_BLESS] - how_much, -8);
	sprintf ((char *) c_line, "  %s cursed.",(char *) name);
	add_string_to_buf((char *) c_line);
		
	put_pc_screen();
	give_help(59,0,0);
}

void pc_record_type::dumbfound(short how_much)
{
	short r1;

	if (!isAlive()) return;
	
	r1 = get_ran(1,0,90);
	
	if (hasAbilEquip(53) < 24) {
		add_string_to_buf("  Ring of Will glows.");
		r1 -= 10;
		}
		
	if (r1 < level)
		how_much -= 2;
		
	if (how_much <= 0) {
		sprintf ((char *) c_line, "  %s saved.",(char *) name);
		add_string_to_buf((char *) c_line);
		return;
		}
		
	status[STATUS_DUMB] = min(status[STATUS_DUMB] + how_much, 8);
	sprintf ((char *) c_line, "  %s dumbfounded.",(char *) name);
	add_string_to_buf((char *) c_line);

	one_sound(67);
	put_pc_screen();
	adjust_spell_menus();
	give_help(28,0,0);
}

void pc_record_type::disease(short how_much)
{
	short r1, tlevel;

	if (!isAlive()) return;
	
	r1 = get_ran(1,0,100);
	
	if (r1 < level * 2)
		how_much -= 2;
		
	if (how_much <= 0)
	{
		sprintf ((char *) c_line, "  %s saved.",(char *) name);
		add_string_to_buf((char *) c_line);
		return;
	}

	if ((tlevel = getProtLevel(62)) > 0)
		how_much -= tlevel / 2;
		
	if ((traits[12] == TRUE) && (how_much > 1))	how_much++;
	if ((traits[12] == TRUE) && (how_much == 1) && (get_ran(1,0,1) == 0)) how_much++;
	
	status[STATUS_DISEASE] = min(status[STATUS_DISEASE] + how_much,8);
	sprintf ((char *) c_line, "  %s diseased.",(char *) name);
	add_string_to_buf((char *) c_line);

	one_sound(66);
	put_pc_screen();
	give_help(29,0,0);
}


void pc_record_type::sleep(short how_much,short what_type,short adjust)
// higher adjust, less chance of saving
{
	short r1, tlevel;
	if (!isAlive()) return;
	
	if (how_much == 0)
		return;
		
	if ((what_type == STATUS_ASLEEP) || (what_type == STATUS_PARALYZED)) { ////
		if ((tlevel = getProtLevel(53)) > 0)
			how_much -= tlevel / 2;
		if ((tlevel = getProtLevel(54)) > 0)
			how_much -= (what_type == STATUS_ASLEEP) ? tlevel : tlevel * 300;

		}
		
	r1 = get_ran(1,0,100) + adjust;
	if (r1 < 30 + level * 2)
		how_much = -1;
	if ((what_type == STATUS_ASLEEP) && ((traits[7] > 0) || (status[STATUS_ASLEEP] < 0)))
		how_much = -1;
	if (how_much <= 0) {
		sprintf ((char *) c_line, "  %s resisted.",(char *) name);
		add_string_to_buf((char *) c_line);
		return;
		}
	if (isAlive()) {
		status[what_type] = how_much;
		if (what_type == STATUS_ASLEEP)
			sprintf ((char *) c_line, "  %s falls asleep.",(char *) name);
			else sprintf ((char *) c_line, "  %s paralyzed.",(char *) name);
		if (what_type == STATUS_ASLEEP)
			play_sound(96);
			else play_sound(90);
		add_string_to_buf((char *) c_line);
		pc_moves[getNum()] = 0;
		}
	put_pc_screen();
	if (what_type == STATUS_ASLEEP)
		give_help(30,0,0);
		else give_help(32,0,0);
}

void pc_record_type::slow(short how_much)
{
	if (!isAlive()) return;
	
	status[STATUS_HASTE] = minmax(-8,8,status[STATUS_HASTE] - how_much);
	if (how_much < 0)
		sprintf ((char *) c_line, "  %s hasted.",(char *) name);
		else sprintf ((char *) c_line, "  %s slowed.",(char *) name);
	add_string_to_buf((char *) c_line);

	put_pc_screen();
	if (how_much < 0)
		give_help(35,0,0);
}

void pc_record_type::web(short how_much)
{
	if (!isAlive()) return;
	
	status[STATUS_WEBS] = min(status[STATUS_WEBS] + how_much,8);
	sprintf ((char *) c_line, "  %s webbed.",(char *) name);
	add_string_to_buf((char *) c_line);
	one_sound(17);

	put_pc_screen();
	give_help(31,0,0);
}

void pc_record_type::acid(short how_much)
{
	if (!isAlive()) return;
	
	if (hasAbilEquip(122) < 24) {
		sprintf ((char *) c_line, "  %s resists acid.",(char *) name);
		add_string_to_buf((char *) c_line);
		return;
		}
		
	status[STATUS_ACID] += how_much;
	sprintf ((char *) c_line, "  %s covered with acid!",(char *) name);
	add_string_to_buf((char *) c_line);
	one_sound(42);
	
	put_pc_screen();
}

void pc_record_type::heal(short amt)
{
	if (!isAlive())	return;
	if (cur_health > max_health) return;
	
	cur_health += amt;
	
	if (cur_health > max_health)
		cur_health = max_health;
}

void pc_record_type::sortItems()
{
	item_record_type store_item;
	short item_priority[26] = {20,8,8,20,9, 9,3,2,1,0, 7,20,10,10,10, 10,10,10,5,6, 4,11,12,9,9, 9};
	bool no_swaps = false, store_equip;
	int i;
	
	while (no_swaps == false) {
		no_swaps = true;
		for (i = 0; i < 23; i++)
			if (item_priority[items[i + 1].variety] < 
			  item_priority[items[i].variety]) {
			  	no_swaps = false;
			  	store_item = items[i + 1];
			  	items[i + 1] = items[i];
			  	items[i] = store_item;
			  	store_equip = equip[i + 1];
	 			equip[i + 1] = equip[i];
				equip[i] = store_equip;
				if (weap_poisoned == i + 1)
					weap_poisoned--;
					else if (weap_poisoned == i)
						weap_poisoned++;
			  	}
		}
}

short pc_record_type::getNum()
{
	short i;
	
	for (i = 0; i < NUM_OF_PCS; i++)
	{
		if (&adven[i] == this) break;;
	}
	
	return i;
}

bool pc_record_type::giveToPC(item_record_type item, bool print_result)
{
	short free_space;
	char announce_string[60];
	
	if (item.variety == 0)
		return true;
		
	if (item.variety == 3) {
		party.gold += item.item_level;
		ASB("You get some gold.");
		return true;
		}
	if (item.variety == 11) {
		party.food += item.item_level;
		ASB("You get some food.");
		return true;
		}
	if (item_weight(item) >  amountCanCarry() - amountCarried()) {
	  	if (print_result == true) {
		  	MessageBeep(MB_OK);
			ASB("Item too heavy to carry.");
			}
		return false;
	  	}
	if (((free_space = hasSpace()) == 24) || (!isAlive()))
		return false;
	else {
			item.item_properties = item.item_properties & 253; // not property
			item.item_properties = item.item_properties & 247; // not contained
			items[free_space] = item;


			if (print_result == 1) {
				if (stat_window == getNum())
					put_item_screen(stat_window,0);
			}
			if (in_startup_mode == FALSE) {
				if (item.isIdent() == false)
					sprintf((char *) announce_string,"  %s gets %s.",name,item.name);
					else sprintf((char *) announce_string,"  %s gets %s.",name,item.full_name);
				if (print_result)
					add_string_to_buf((char *)announce_string);
				}

			combineThings();
			sortItems();
			return true;
			}
	return false;
}

// returnes equipped protection level of specified abil, or -1 if no such abil is equipped
short pc_record_type::getProtLevel(short abil)
{
	for (int i = 0; i < 24; i++)
		if ((items[i].variety != 0) && (items[i].ability == abil) && (equip[i] == TRUE))
				return items[i].ability_strength;
	return (-1);	
}

short pc_record_type::hasAbilEquip(short abil)
{
	short i = 0;
	
	while (((items[i].variety == 0) || (items[i].ability != abil)
			|| (equip[i] == FALSE)) && (i < 24))
				i++;
	return i;
}

short pc_record_type::hasAbil(short abil)
{
	short i = 0;
	
	while (((items[i].variety == 0) || (items[i].ability != abil)) && (i < 24)) i++;
	return i;
}

short pc_record_type::amountCanCarry()
{
	return 100 + (15 * min(skills[0],20)) + ((traits[8] == 0) ? 0 : 30)
		+ ((traits[14] == 0) ? 0 : -50);
}

short pc_record_type::amountCarried()
{
	short i, storage = 0;
	bool airy = false, heavy = false;
	
	for (i = 0; i < 24; i++) 
		if (items[i].variety > 0)
		{
			storage += item_weight(items[i]);
			if (items[i].ability == 44) airy = true;
			if (items[i].ability == 45)	heavy = true;
		}
		
	if (airy) storage -= 30;
	if (heavy) storage += 30;
	if (storage < 0) storage = 0;
	
	return storage;
}

short pc_record_type::hasSpace()
{
	int i = 0;
	
	while (i < 24)
	{
	if (items[i].variety == 0)
		return i;
	i++;
	}
	
	return 24;
}

// returns 1 if OK, 2 if no room, 3 if not enough cash, 4 if too heavy, 5 if too many of item
short pc_record_type::okToBuy(short cost, item_record_type item)
{
	int i;
	
	if ((item.variety != 3) && (item.variety != 11)) {
		for (i = 0; i < 24; i++)
			if ((items[i].variety > 0) && (items[i].type_flag == item.type_flag)
				&& (items[i].charges > 123))
					return 5;

		if (hasSpace() == 24) return 2;
		if (item_weight(item) > amountCanCarry() - amountCarried()) return 4;
	  	}
	  	
	if (party.takeGold(cost, false) == false)
		return 3;
	return 1;
}

void pc_record_type::takeItem(short which_item)
//short pc_num,which_item;  // if which_item > 30, don't update stat win, item is which_item - 30
{
	int i;
	bool do_print = true;
	
	if (which_item >= 30) {
		do_print = false;
		which_item -= 30;
		}
		
	if ((weap_poisoned == which_item) && (status[0] > 0)) {
			add_string_to_buf("  Poison lost.           ");
			status[0] = 0;
		}
		
	if ((weap_poisoned > which_item) && (status[0] > 0)) 
		weap_poisoned--;
		
	for (i = which_item; i < 23; i++) {
		items[i] = items[i + 1];
		equip[i] = equip[i + 1];
		}
	items[23] = return_dummy_item();
	equip[23] = FALSE;
	
	if ((stat_window == getNum()) && (do_print))
		put_item_screen(stat_window,1);
}

void pc_record_type::removeCharge(short which_item)
{
	if (items[which_item].charges > 0)
	{
		items[which_item].charges--;
		if (items[which_item].charges == 0) takeItem(which_item);
	}

	if (stat_window == getNum())
		put_item_screen(stat_window,1);

}

void pc_record_type::enchantWeapon(short item_hit, short enchant_type, short new_val)
{
	char store_name[60];

	if ((items[item_hit].isMagic()) || (items[item_hit].ability != 0)) return;
	
	items[item_hit].item_properties |= 4;
	
	switch (enchant_type) {
		case 0:
			sprintf((char *)store_name,"%s (+1)", items[item_hit].full_name);
			items[item_hit].bonus++;
			items[item_hit].value = new_val;
			break;
		case 1:
			sprintf((char *)store_name,"%s (+2)", items[item_hit].full_name);
			items[item_hit].bonus += 2;
			items[item_hit].value = new_val;
			break;
		case 2:
			sprintf((char *)store_name,"%s (+3)", items[item_hit].full_name);
			items[item_hit].bonus += 3;
			items[item_hit].value = new_val;
			break;
		case 3:
			sprintf((char *)store_name,"%s (F)", items[item_hit].full_name);
			items[item_hit].ability = 110;
			items[item_hit].ability_strength = 5;
			items[item_hit].charges = 8;
			break;
		case 4:
			sprintf((char *)store_name,"%s (F!)", items[item_hit].full_name);
			items[item_hit].value = new_val;
			items[item_hit].ability = 1;
			items[item_hit].ability_strength = 5;
			break;
		case 5:
			sprintf((char *)store_name,"%s (+5)", items[item_hit].full_name);
			items[item_hit].value = new_val;
			items[item_hit].bonus += 5;
			break;
		case 6:
			sprintf((char *)store_name,"%s (B)", items[item_hit].full_name);
			items[item_hit].bonus++;
			items[item_hit].ability = 71;
			items[item_hit].ability_strength = 5;
			items[item_hit].magic_use_type = 0;
			items[item_hit].charges = 8;
			break;
		default:
			strcpy(store_name, items[item_hit].full_name);
			break;
		}
	if (items[item_hit].value > 15000)
		items[item_hit].value = 15000;
	if (items[item_hit].value < 0)
		items[item_hit].value = 15000;
	strcpy(items[item_hit].full_name,store_name);
}

void pc_record_type::equipItem(short item_num)
{
	short num_equipped_of_this_type = 0;
	short num_hands_occupied = 0;
	short i;
	short equip_item_type = 0;
	
	if ((overall_mode == MODE_COMBAT) && (items[item_num].variety == 11))
		add_string_to_buf("Equip: Not in combat");
	else {
		// unequip
	if (equip[item_num] == TRUE) {
		if ((equip[item_num] == TRUE) &&
			(items[item_num].isCursed()))
			add_string_to_buf("Equip: Item is cursed.           ");
  			else {
				equip[item_num] = FALSE;
				add_string_to_buf("Equip: Unequipped");
				if ((weap_poisoned == item_num) && (status[0] > 0)) {
						add_string_to_buf("  Poison lost.           ");
						status[0] = 0;
					}
			}
		}

	else {  // equip
		if (equippable[items[item_num].variety] == FALSE)
			add_string_to_buf("Equip: Can't equip this item.");
				else {
					for (i = 0; i < 24; i++) 
						if (equip[i] == TRUE) {
							if (items[i].variety == items[item_num].variety)
								num_equipped_of_this_type++;
							num_hands_occupied = num_hands_occupied + num_hands_to_use[items[i].variety];
						}
						
						
					equip_item_type = excluding_types[items[item_num].variety];
					// Now if missile is already equipped, no more missiles
					if (equip_item_type > 0) {
						for (i = 0; i < 24; i++) 
							if ((equip[i] == TRUE) && (excluding_types[items[i].variety] == equip_item_type)) {
								add_string_to_buf("Equip: You have something of");
								add_string_to_buf("  this type equipped.");
								return;
								}
						}

					if ((is_combat()) && (items[item_num].variety == 13))
						add_string_to_buf("Equip: Not armor in combat");
						else if ((2 - num_hands_occupied) < num_hands_to_use[items[item_num].variety])
							add_string_to_buf("Equip: Not enough free hands");
							else if (num_that_can_equip[items[item_num].variety] <= num_equipped_of_this_type)
								add_string_to_buf("Equip: Can't equip another");
								else {
									equip[item_num] = TRUE;
									add_string_to_buf("Equip: OK");
									}
					}
			
		}
	}

	if (stat_window == getNum())
		put_item_screen(stat_window,1);
}


void pc_record_type::dropItem(short item_num, location where_drop)
{
	short choice, how_many = 0;
	item_record_type item_store;
	bool take_given_item = true;
	location loc;
	
	item_store = items[item_num];

	if ((equip[item_num] == TRUE) && (items[item_num].isCursed())) 
			add_string_to_buf("Drop: Item is cursed.           ");	
	else switch (overall_mode) {
		case 0:
			choice = fancy_choice_dialog(1093,0);
			if (choice == 1)
				return;
			add_string_to_buf("Drop: OK");
			if ((item_store.type_flag > 0) && (item_store.charges > 1)) {
				how_many = get_num_of_items(item_store.charges);
				if (how_many == item_store.charges)
					takeItem(item_num);
					else items[item_num].charges -= how_many;
				}
				else takeItem(item_num);
			break;
		
		case 5: case 15:
			loc = where_drop;
			if ((item_store.type_flag > 0) && (item_store.charges > 1)) {
				how_many = get_num_of_items(item_store.charges);
				if (how_many <= 0)
					return;
				if (how_many < item_store.charges)
					take_given_item = false;
				item_store.charges = how_many;
				}
			if (is_container(loc) == TRUE)
				item_store.item_properties = item_store.item_properties | 8;
			if (place_item(item_store,loc,FALSE) == FALSE) {
				add_string_to_buf("Drop: Too many items on ground");
				item_store.item_properties = item_store.item_properties & 247; // not contained
				}
				else {
					if (item_store.isContained())
						add_string_to_buf("Drop: Item put away");
						else add_string_to_buf("Drop: OK");
					items[item_num].charges -= how_many;
					if (take_given_item) takeItem(item_num);
					}	
			break;
		}
}

void pc_record_type::giveThing(short item_num)
{
	short who_to,how_many = 0;
	item_record_type item_store;
	bool take_given_item = true;
	short pc_num = getNum();
	
	if ((equip[item_num] == TRUE) && (items[item_num].isCursed()))
			add_string_to_buf("Give: Item is cursed.           ");
  			else {
				item_store = items[item_num];
				who_to = char_select_pc(1,1,"Give item to who?");
				if ((overall_mode == MODE_COMBAT) && (adjacent(pc_pos[pc_num],pc_pos[who_to]) == FALSE)) {
					add_string_to_buf("Give: Must be adjacent.");
					who_to = 6;
					}

				if ((who_to < 6) && (who_to != pc_num) 
					&& ((overall_mode != MODE_COMBAT) || (adjacent(pc_pos[pc_num],pc_pos[who_to]) == TRUE))) {
					if ((item_store.type_flag > 0) && (item_store.charges > 1)) {
						how_many = get_num_of_items(item_store.charges);
						if (how_many == 0)
							return;
						if (how_many < item_store.charges)
							take_given_item = false;
						items[item_num].charges -= how_many;
						item_store.charges = how_many;
						}					
					if (adven[who_to].giveToPC(item_store,0) == true) {
						if (take_given_item) takeItem(item_num);	
						}
						else {
							if (adven[who_to].hasSpace() == 24)
								ASB("Can't give: PC has max. # of items.");
								else ASB("Can't give: PC carrying too much.");
							if (how_many > 0)
								items[item_num].charges += how_many;
							}				
				}
		}
}

void pc_record_type::combineThings()
{
	int i,j,test;
	
	for (i = 0; i < 24; i++) {
		if ((items[i].variety > 0) && (items[i].type_flag > 0) && (items[i].isIdent())) 
			{
			for (j = i + 1; j < 24; j++)
				if ((items[j].variety > 0) && (items[j].type_flag == items[i].type_flag) && (items[j].isIdent())) 
				 {
					add_string_to_buf("(items combined)");
					test = items[i].charges + items[j].charges;
					if (test > 125) {
						items[i].charges = 125;
						ASB("(Can have at most 125 of any item.");
						}
				 		else items[i].charges += items[j].charges;
				 	if (equip[j] == TRUE) {
				 		equip[i] = TRUE;
				 		equip[j] = FALSE;
				 		}
					takeItem(30 + j);
				}
			}		
		if ((items[i].variety > 0) && (items[i].charges < 0))
			items[i].charges = 1;
		}
}
