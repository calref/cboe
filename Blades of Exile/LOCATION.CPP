#include "location.h"		// location
#include "exlsound.h"		// play_sound
#include "blades.h"			// out
#include "text.h"			// add_string_to_buf
#include "locutils.h"		// alter_space
#include "items.h"			// fancy_choice_dialog

#include "global.h"
#include "globvar.h"

location location::toGlobal()
{
	//return local_to_global(*this);
	
	location global = *this;
	if (party.i_w_c.x == 1)	global.x = global.x + 48; 
	if (party.i_w_c.y == 1)	global.y = global.y + 48;
	return global; 
}

location location::toLocal()
{
//	return global_to_local(*this);

	location local = (*this);
	if (party.i_w_c.x == 1)	local.x = local.x - 48; 
	if (party.i_w_c.y == 1)	local.y = local.y - 48;
	return local; 
}

short location::countWalls() const
{
	unsigned char walls[31] = {5,6,7,8,9, 10,11,12,13,14, 15,16,17,18,19, 20,21,22,23,24,
							25,26,27,28,29, 30,31,32,33,34, 35};
	short answer = 0;
	short k = 0;

	for (k = 0; k < 31 ; k++)
	{
		if (out[x + 1][y] == walls[k]) answer++;	
		if (out[x - 1][y] == walls[k]) answer++;	
		if (out[x][y + 1] == walls[k]) answer++;	
		if (out[x][y - 1] == walls[k]) answer++;	
	}
	return answer;		
}

// 0 if no pull.
// 1 if in right position now.
// 2 if in left position
// levers should always start to left.
short location::handleLever()
{
	if (FCD(1020,0) == 1) return 0;
	play_sound(94);
	alter_space(x,y,scenario.ter_types[t_d.terrain[x][y]].trans_to_what);
	return 1;
}

void location::crumbleWall()
{
	unsigned char ter;
	
	if (loc_off_act_area(*this) == TRUE)
		return;
	ter = t_d.terrain[x][y];
	if (scenario.ter_types[ter].special == 7) {
			play_sound(60);
			t_d.terrain[x][y] = scenario.ter_types[ter].flag1;
			if(scenario.ter_types[scenario.ter_types[ter].flag1].special >= 16 && scenario.ter_types[scenario.ter_types[ter].flag1].special <=19)
                belt_present = TRUE;
			add_string_to_buf("  Barrier crumbles.");	
		}
}

bool location::isDoor() const
{
	if ((scenario.ter_types[t_d.terrain[x][y]].special == 9) ||
		(scenario.ter_types[t_d.terrain[x][y]].special == 1) ||
		(scenario.ter_types[t_d.terrain[x][y]].special == 10))
			return true;
	return false;
}

//void pick_lock(location where,short pc_num)
void location::pickLock(short pc_num)
{
	unsigned char terrain;
	short r1,which_item;
	Boolean will_break = FALSE;
	short unlock_adjust;
	
	terrain = t_d.terrain[x][y];
	which_item = adven[pc_num].hasAbilEquip(161);
	if (which_item == 24) {
		add_string_to_buf("  Need lockpick equipped.        ");
		return;
		}

	r1 = get_ran(1,0,100) + adven[pc_num].items[which_item].ability_strength * 7;

	if (r1 < 75)
		will_break = TRUE;

	r1 = get_ran(1,0,100) - 5 * adven[pc_num].statAdj(1) + c_town.difficulty * 7
	 - 5 * adven[pc_num].skills[15] - adven[pc_num].items[which_item].ability_strength * 7;

	// Nimble?
	if (adven[pc_num].traits[TRAIT_NIMBLE] == TRUE) r1 -= 8;

	if (adven[pc_num].hasAbilEquip(42) < 24)
		r1 = r1 - 12;	

	if ((scenario.ter_types[terrain].special < 9) || (scenario.ter_types[terrain].special > 10)) {
		add_string_to_buf("  Wrong terrain type.           ");
		return;
		}
	unlock_adjust = scenario.ter_types[terrain].flag2;
	if ((unlock_adjust >= 5) || (r1 > (unlock_adjust * 15 + 30))) {
					add_string_to_buf("  Didn't work.                ");
					if (will_break == TRUE)
					{
						add_string_to_buf("  Pick breaks.                ");
						adven[pc_num].removeCharge(which_item);
					}
				play_sound(41);
				} 
				else {
						add_string_to_buf("  Door unlocked.                ");
						play_sound(9);
						t_d.terrain[x][y] = scenario.ter_types[terrain].flag1;
						if(scenario.ter_types[scenario.ter_types[terrain].flag1].special >= 16 && scenario.ter_types[scenario.ter_types[terrain].flag1].special <=19)
                            belt_present = TRUE;
					}
}

//void bash_door(location where,short pc_num)
void location::bashDoor(short pc_num)
{
	unsigned char terrain;
	short r1,unlock_adjust;

	terrain = t_d.terrain[x][y];
	r1 = get_ran(1,0,100) - 15 * adven[pc_num].statAdj(0) + c_town.difficulty * 4;
	
	if ((scenario.ter_types[terrain].special < 9) || (scenario.ter_types[terrain].special > 10))
	{
		add_string_to_buf("  Wrong terrain type.           ");
		return;
	}

	unlock_adjust = scenario.ter_types[terrain].flag2;
	
	if ((unlock_adjust >= 5) || (r1 > (unlock_adjust * 15 + 40)) || (scenario.ter_types[terrain].special != 10))
	{
		add_string_to_buf("  Didn't work.                ");
		adven[pc_num].damage(get_ran(1,1,4),4,-1);
	} 
	else
	{
		add_string_to_buf("  Lock breaks.                ");
		play_sound(9);
		t_d.terrain[x][y] = scenario.ter_types[terrain].flag1;
		if(scenario.ter_types[scenario.ter_types[terrain].flag1].special >= 16 && scenario.ter_types[scenario.ter_types[terrain].flag1].special <=19)
            belt_present = TRUE;
	}
}
