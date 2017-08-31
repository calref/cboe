
#include "boe.global.hpp"

#include "universe.hpp"
#include "boe.party.hpp"
#include "boe.town.hpp"
#include "boe.items.hpp"
#include "boe.combat.hpp"
#include "boe.monster.hpp"
#include "boe.infodlg.hpp"
#include "boe.locutils.hpp"
#include "boe.text.hpp"
#include "boe.townspec.hpp"
#include "sounds.hpp"
#include "mathutil.hpp"
#include "strdlog.hpp"
#include "choicedlog.hpp"
#include "winutil.hpp"
#include "boe.menus.hpp"
#include "utility.hpp"

extern eGameMode overall_mode;
extern short stat_window;
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

//short trap_type; // 0 - random  1 - blade  2 - dart  3 - gas  4 - boom  5 - paralyze  6  - no
// 7 - level drain  8 - alert  9 - big flames 10 - dumbfound 11 - disease 1
// 12 - disease all
bool run_trap(short pc_num,eTrapType trap_type,short trap_level,short diff) {
	short r1,skill,num_hits = 1;
	short trap_odds[30] = {5,30,35,42,48, 55,63,69,75,77,
		78,80,82,84,86, 88,90,92,94,96,98,99,99,99,99,99,99,99,99,99};
	
	num_hits += trap_level;
	
	if(trap_type == TRAP_RANDOM)
		trap_type = (eTrapType) get_ran(1,1,4);
	if(trap_type == TRAP_FALSE_ALARM)
		return true;
	
	cPlayer& disarmer = univ.party[pc_num];
	
	if(pc_num < 6) {
		int i = disarmer.stat_adj(eSkill::DEXTERITY);
		i += disarmer.get_prot_level(eItemAbil::THIEVING) / 2;
		skill = minmax(0,20,disarmer.skill(eSkill::DISARM_TRAPS) +
					   + disarmer.skill(eSkill::LUCK) / 2 + 1 - univ.town.difficulty + 2 * i);
		
		r1 = get_ran(1,1,100) + diff;
		// Nimble?
		if(disarmer.traits[eTrait::NIMBLE])
			r1 -= 6;
		
		
		if(r1 < trap_odds[skill]) {
			add_string_to_buf("  Trap disarmed.");
			return true;
		}
		else add_string_to_buf("  Disarm failed.");
	}
	
	switch(trap_type) {
		case TRAP_BLADE:
			for(short i = 0; i < num_hits; i++) {
				add_string_to_buf("  A knife flies out!");
				r1 = get_ran(2 + univ.town.difficulty / 14,1,10);
				damage_pc(disarmer,r1,eDamageType::WEAPON,eRace::UNKNOWN,0);
			}
			break;
			
		case TRAP_DART:
			add_string_to_buf("  A dart flies out.");
			r1 = 3 + univ.town.difficulty / 14;
			r1 = r1 + trap_level * 2;
			disarmer.poison(r1);
			break;
			
		case TRAP_GAS:
			add_string_to_buf("  Poison gas pours out.");
			r1 = 2 + univ.town.difficulty / 14;
			r1 = r1 + trap_level * 2;
			univ.party.poison(r1);
			break;
			
		case TRAP_EXPLOSION:
			for(short i = 0; i < num_hits; i++) {
				add_string_to_buf("  There is an explosion.");
				r1 = get_ran(3 + univ.town.difficulty / 13,1,8);
				hit_party(r1,eDamageType::FIRE);
			}
			break;
			
		case TRAP_SLEEP_RAY:
			add_string_to_buf("  A purple ray flies out.");
			r1 = 200 + univ.town.difficulty * 100;
			r1 = r1 + trap_level * 400;
			// TODO: It says sleep ray but is actually paralysis ray?
			disarmer.sleep(eStatus::PARALYZED, r1, 50);
			break;
		case TRAP_DRAIN_XP:
			add_string_to_buf("  You feel weak.");
			r1 = 40;
			r1 = r1 + trap_level * 30;
			disarmer.experience = max(0,disarmer.experience - r1);
			break;
			
		case TRAP_ALERT:
			add_string_to_buf("  An alarm goes off!!!");
			make_town_hostile();
			break;
			
		case TRAP_FLAMES:
			add_string_to_buf("  Flames shoot from the walls.");
			r1 = get_ran(10 + trap_level * 5,1,8);
			hit_party(r1,eDamageType::FIRE);
			break;
		case TRAP_DUMBFOUND:
			add_string_to_buf("  You feel disoriented.");
			univ.party.dumbfound(2 + trap_level * 2);
			adjust_spell_menus();
			break;
			
		case TRAP_DISEASE:
			add_string_to_buf("  You prick your finger.");
			r1 = 3 + univ.town.difficulty / 14;
			r1 = r1 + trap_level * 2;
			disarmer.disease(r1);
			break;
			
		case TRAP_DISEASE_ALL:
			add_string_to_buf("  A foul substance sprays out.");
			r1 = 2 + univ.town.difficulty / 14;
			r1 = r1 + trap_level * 2;
			univ.party.disease(r1);
			break;
			
		case TRAP_CUSTOM:
			univ.party.force_ptr(15, trap_level);
			break;
			
		default:
			add_string_to_buf("ERROR: Invalid trap type."); // should never be reached
	}
	put_pc_screen();
	put_item_screen(stat_window);
	return false;
}

location get_spec_loc(short which) {
	location where;
	
	for(short i = 0; i < univ.town->special_locs.size(); i++)
		if(univ.town->special_locs[i].spec == which)
			return univ.town->special_locs[i];
	return where;
}

bool handle_lever(location w) {
	if(cChoiceDlog("basic-lever",{"pull", "leave"}).show() == "leave")
		return false;
	play_sound(94);
	alter_space(w.x,w.y,univ.scenario.ter_types[univ.town->terrain(w.x,w.y)].trans_to_what);
	return true;
}

