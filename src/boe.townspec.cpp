
#include "boe.global.h"

#include "classes.h"
#include "boe.party.h"
#include "boe.town.h"
#include "boe.items.h"
#include "boe.combat.h"
#include "boe.monster.h"
#include "boe.infodlg.h"
#include "boe.locutils.h"
#include "boe.text.h"
#include "boe.itemdata.h"
#include "boe.townspec.h"
#include "soundtool.hpp"
#include "mathutil.hpp"
#include "dlogutil.hpp"
#include "winutil.hpp"

extern eGameMode overall_mode;
extern short current_pc,stat_window;
extern location center;
extern sf::RenderWindow mainPtr;
extern cUniverse univ;
char answer[256];

void activate_monster_enc(short enc_num,std::string list,short str,short strsnd,short *flip_bit) {
	if(*flip_bit == 0) {
		cStrDlog display_enc_string(get_str(list,str),"","",8,PIC_DLOG);
		display_enc_string.setSound(strsnd);
		display_enc_string.show();
		activate_monsters(enc_num,1);
		*flip_bit = 20;
	}
}

//short pc_num; // 6 - BOOM!  7 - pick here
//short trap_type; // 0 - random  1 - blade  2 - dart  3 - gas  4 - boom  5 - paralyze  6  - no
// 7 - level drain  8 - alert  9 - big flames 10 - dumbfound 11 - disease 1
// 12 - disease all
bool run_trap(short pc_num,eTrapType trap_type,short trap_level,short diff) {
	short r1,skill,i,num_hits = 1,i_level;
	short trap_odds[30] = {5,30,35,42,48, 55,63,69,75,77,
		78,80,82,84,86, 88,90,92,94,96,98,99,99,99,99,99,99,99,99,99};
	
	if(pc_num > 7) { // Debug
		beep();
		ASB("TRAP ERROR! REPORT!");
		return true;
	}
	
	if(pc_num == 7) {
		pc_num = select_pc(1,0);
		if(pc_num == 6)
			return false;
	}
	
	
	num_hits += trap_level;
	
	if(trap_type == TRAP_RANDOM)
		trap_type = (eTrapType) get_ran(1,1,4);
	if(trap_type == TRAP_FALSE_ALARM)
		return true;
	
	if(pc_num < 6) {
		i = stat_adj(pc_num,eSkill::DEXTERITY);
		if((i_level = get_prot_level(pc_num,eItemAbil::THIEVING)) > 0)
			i = i + i_level / 2;
		skill = minmax(0,20,univ.party[pc_num].skills[eSkill::DISARM_TRAPS] +
					   + univ.party[pc_num].skills[eSkill::LUCK] / 2 + 1 - univ.town.difficulty + 2 * i);
		
		r1 = get_ran(1,1,100) + diff;
		// Nimble?
		if(univ.party[pc_num].traits[eTrait::NIMBLE])
			r1 -= 6;
		
		
		if(r1 < trap_odds[skill]) {
			add_string_to_buf("  Trap disarmed.            ");
			return true;
		}
		else add_string_to_buf("  Disarm failed.          ");
	}
	
	switch(trap_type) {
		case TRAP_BLADE:
			for(i = 0; i < num_hits; i++) {
				add_string_to_buf("  A knife flies out!              ");
				r1 = get_ran(2 + univ.town.difficulty / 14,1,10);
				damage_pc(pc_num,r1,eDamageType::WEAPON,eRace::UNKNOWN,0);
			}
			break;
			
		case TRAP_DART:
			add_string_to_buf("  A dart flies out.              ");
			r1 = 3 + univ.town.difficulty / 14;
			r1 = r1 + trap_level * 2;
			poison_pc(pc_num,r1);
			break;
			
		case TRAP_GAS:
			add_string_to_buf("  Poison gas pours out.          ");
			r1 = 2 + univ.town.difficulty / 14;
			r1 = r1 + trap_level * 2;
			for(i = 0; i < 6; i++)
				poison_pc(i,r1);
			break;
			
		case TRAP_EXPLOSION:
			for(i = 0; i < num_hits; i++) {
				add_string_to_buf("  There is an explosion.        ");
				r1 = get_ran(3 + univ.town.difficulty / 13,1,8);
				hit_party(r1,eDamageType::FIRE);
			}
			break;
			
		case TRAP_SLEEP_RAY:
			add_string_to_buf("  A purple ray flies out.          ");
			r1 = 200 + univ.town.difficulty * 100;
			r1 = r1 + trap_level * 400;
			sleep_pc(pc_num,r1,eStatus::PARALYZED,50);
			break;
		case TRAP_DRAIN_XP:
			add_string_to_buf("  You feel weak.            ");
			r1 = 40;
			r1 = r1 + trap_level * 30;
			univ.party[pc_num].experience = max (0,univ.party[pc_num].experience - r1);
			break;
			
		case TRAP_ALERT:
			add_string_to_buf("  An alarm goes off!!!            ");
			make_town_hostile();
			break;
			
		case TRAP_FLAMES:
			add_string_to_buf("  Flames shoot from the walls.        ");
			r1 = get_ran(10 + trap_level * 5,1,8);
			hit_party(r1,eDamageType::FIRE);
			break;
		case TRAP_DUMBFOUND:
			add_string_to_buf("  You feel disoriented.        ");
			for(i = 0; i < 6; i++)
				dumbfound_pc(i,2 + trap_level * 2);
			break;
			
		case TRAP_DISEASE:
			add_string_to_buf("  You prick your finger. ");
			r1 = 3 + univ.town.difficulty / 14;
			r1 = r1 + trap_level * 2;
			disease_pc(pc_num,r1);
			break;
			
		case TRAP_DISEASE_ALL:
			add_string_to_buf("  A foul substance sprays out.");
			r1 = 2 + univ.town.difficulty / 14;
			r1 = r1 + trap_level * 2;
			for(i = 0; i < 6; i++)
				disease_pc(i,r1);
			break;
			
		default:
			add_string_to_buf("ERROR: Invalid trap type."); // should never be reached
	}
	put_pc_screen();
	put_item_screen(stat_window,0);
	return true;
}

location get_spec_loc(short which) {
	location where;
	short i;
	
	for(i = 0; i < 50; i++)
		if(univ.town->spec_id[i] == which)
			return univ.town->special_locs[i];
	return where;
}

// 0 if no pull.
// 1 if pull
// levers should always start to left.
short handle_lever(location w) {
	if(cChoiceDlog("basic-lever",{"pull", "leave"}).show() == "leave")
		return 0;
	play_sound(94);
	switch_lever(w);
	return 1;
}

void switch_lever(location w) {
	alter_space(w.x,w.y,univ.scenario.ter_types[univ.town->terrain(w.x,w.y)].trans_to_what);
}

