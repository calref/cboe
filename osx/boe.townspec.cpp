

//#include "item.h"

#include "boe.global.h"
using namespace std;
#include "classes.h"
#include "boe.party.h"
#include "boe.town.h"
#include "boe.items.h"
#include "boe.combat.h"
#include "boe.monster.h"
#include "boe.infodlg.h"
#include "boe.locutils.h"
#include "boe.fields.h"
#include "boe.text.h"
#include "boe.itemdata.h"
#include "boe.townspec.h"
#include "soundtool.h"
#include "mathutil.h"
#include "dlgutil.h"

extern eGameMode overall_mode;
//extern party_record_type univ.party;
//extern current_town_type	univ.town;
//extern unsigned char out[96][96],out_e[96][96];
extern unsigned short combat_terrain[64][64];
extern short current_pc,stat_window;
//extern cOutdoors outdoors[2][2];
extern location pc_pos[6],center;
//extern town_item_list	t_i;
//extern pc_record_type ADVEN[6];
//extern big_tr_type t_d;
extern WindowPtr mainPtr;
extern cScenario scenario;
extern cUniverse univ;
Str255 answer;

/*bool (which)
short which;
{
	short choice,i;
	item_record_type treas[] = {
{0,0,0,0,0,0,0,0,0,0,0,0,false,false,false,{0,0},"",""}	
		};
	bool can_enter = true;
	location where;
	
	where = get_spec_loc(which);

	switch (which) {
		case 1:
			break;
	
	
		}
		
	return can_enter;
} */




void activate_monster_enc(short enc_num,short str1,short str2,short strsnd,short *flip_bit)
{
		if (*flip_bit == 0) {
			display_enc_string(str1,str2,strsnd);
			activate_monsters(enc_num,1);
			*flip_bit = 20;
			}
}


// OTS = One Time Strings
void OTS(short str1b,short str2b,short str12a,short sound,unsigned char *flip_bit)
{
	if (*flip_bit > 0)
		return;
		else *flip_bit = 20;
	if (str2b == 0)
		D2ES(str12a,str1b,0,0,sound);
		else D2ES(str12a,str1b,str12a,str2b,sound);
}

// GFI go for it ... do simple dialog check
bool GFI(short dialog_num)
{
	if (FCD(dialog_num,0) > 1)
		return true;
		else return false;
}

//DSG = Do Special Give
void DSG(short item_num,unsigned char *flip_bit,short dialog_num,short what_spec,short amt_gold,short amt_food)
//short special;  // 0 - nah  3** - Set spec item 1000 + * give food  2000 + * give cash
{
	short choice;
	bool did_give;
	cItemRec item;

	item = get_stored_item(item_num);	
	if (item_num == 0)
		item.variety = ITEM_TYPE_NO_ITEM;
	if (*flip_bit == 0) {
			choice = fancy_choice_dialog(dialog_num,0);
			if (choice == 1)
				return;
			
			if (item.variety == 0)
				did_give = true;
			else did_give = give_to_party(item,0);
			if (did_give == true) {
				univ.party.food += amt_food;
				univ.party.gold += amt_gold;
				if (what_spec >= 0) {
					if (univ.party.spec_items[what_spec] > 0) {
						ASB("You already have this special item.");
						return;
						}
					univ.party.spec_items[what_spec] += 1;
					put_item_screen(stat_window,0);
					}
				*flip_bit = 20;
				put_pc_screen();
				}
				else choice = fancy_choice_dialog(1049,0);
		}
}


bool run_trap(short pc_num,eTrapType trap_type,short trap_level,short diff)
//short pc_num; // 6 - BOOM!  7 - pick here
//short trap_type; // 0 - random  1 - blade  2 - dart  3 - gas  4 - boom  5 - paralyze  6  - no   
				 // 7 - level drain  8 - alert  9 - big flames 10 - dumbfound 11 - disease 1
				 // 12 - disease all
{
	short r1,skill,i,num_hits = 1,i_level;
	short trap_odds[30] = {5,30,35,42,48, 55,63,69,75,77,
							78,80,82,84,86, 88,90,92,94,96,98,99,99,99,99,99,99,99,99,99};
	
	if (pc_num > 7) { // Debug
		SysBeep(50);
		ASB("TRAP ERROR! REPORT!");
		return true;
		}
	
	if (pc_num == 7) {
		pc_num = select_pc(1,0);
		if (pc_num == 6)
			return false;
		}

	
	num_hits += trap_level;
		
	if (trap_type == TRAP_RANDOM)
		trap_type = (eTrapType) get_ran(1,1,4);
	if (trap_type == TRAP_FALSE_ALARM)
		return true;
		
	if (pc_num < 6) {
			i = stat_adj(pc_num,1);
			if ((i_level = get_prot_level(pc_num,42)) > 0)
				i = i + i_level / 2;
			skill = minmax(0,20,ADVEN[pc_num].skills[SKILL_DISARM_TRAPS] + 
				+ ADVEN[pc_num].skills[SKILL_LUCK] / 2 + 1 - univ.town.difficulty + 2 * i);
	
			r1 = get_ran(1,1,100) + diff;
			// Nimble?
			if (ADVEN[pc_num].traits[TRAIT_NIMBLE])
				r1 -= 6;


			if (r1 < trap_odds[skill]) {
				add_string_to_buf("  Trap disarmed.            ");
				return true;
			}
			else add_string_to_buf("  Disarm failed.          ");
		}
	
	switch (trap_type) {
		case TRAP_BLADE:
			for (i = 0; i < num_hits; i++) {
					add_string_to_buf("  A knife flies out!              ");
					r1 = get_ran(2 + univ.town.difficulty / 14,1,10);
					damage_pc(pc_num,r1,DAMAGE_WEAPON,MONSTER_TYPE_UNKNOWN,0);
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
			for (i = 0; i < num_hits; i++) {
					add_string_to_buf("  There is an explosion.        ");
					r1 = get_ran(3 + univ.town.difficulty / 13,1,8);
					hit_party(r1,DAMAGE_FIRE);
				}
			break;
			
		case TRAP_SLEEP_RAY:
			add_string_to_buf("  A purple ray flies out.          ");
			r1 = 200 + univ.town.difficulty * 100;
			r1 = r1 + trap_level * 400;
			sleep_pc(pc_num,r1,12,50);
			break;
		case TRAP_DRAIN_XP:
			add_string_to_buf("  You feel weak.            ");
			r1 = 40;
			r1 = r1 + trap_level * 30;
			ADVEN[pc_num].experience = max (0,ADVEN[pc_num].experience - r1);
			break;
		
		case TRAP_ALERT:
			add_string_to_buf("  An alarm goes off!!!            ");
			make_town_hostile();
			break;

		case TRAP_FLAMES: 
			add_string_to_buf("  Flames shoot from the walls.        ");
			r1 = get_ran(10 + trap_level * 5,1,8);
			hit_party(r1,DAMAGE_FIRE);
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

location get_spec_loc(short which)
{
	location where;
	short i;
	
	for (i = 0; i < 50; i++)
		if (univ.town->spec_id[i] == which)
			return univ.town->special_locs[i];
	return where;
}


void start_split(short a,short b,short noise) 
{
	short i;

	PSD[SDF_IS_PARTY_SPLIT] = 1;
	PSD[SDF_PARTY_SPLIT_X] = univ.town.p_loc.x;
	PSD[SDF_PARTY_SPLIT_Y] = univ.town.p_loc.y;
	PSD[SDF_PARTY_SPLIT_TOWN] = univ.town.num;
	univ.town.p_loc.x = a;
	univ.town.p_loc.y = b;
	for (i = 0; i < 6; i++)
		if (i != PSD[SDF_PARTY_SPLIT_PC])
			ADVEN[i].main_status += MAIN_STATUS_SPLIT;
	current_pc = PSD[SDF_PARTY_SPLIT_PC];
	update_explored(univ.town.p_loc);
	center = univ.town.p_loc;
	if (noise > 0)
		play_sound(10);	
}

void end_split(short noise) 
{
	short i;

	if (PSD[SDF_IS_PARTY_SPLIT] == 0) {
		ASB("Party already together!");
		return;
		}
	univ.town.p_loc.x = PSD[SDF_PARTY_SPLIT_X];
	univ.town.p_loc.y = PSD[SDF_PARTY_SPLIT_Y];
	PSD[SDF_IS_PARTY_SPLIT] = 0;
	for (i = 0; i < 6; i++)
		if (ADVEN[i].main_status >= MAIN_STATUS_SPLIT)
			ADVEN[i].main_status -= MAIN_STATUS_SPLIT;
	update_explored(univ.town.p_loc);
	center = univ.town.p_loc;
	if (noise > 0)
		play_sound(10);	
	add_string_to_buf("You are reunited.");
}

// 0 if no pull.
// 1 if pull
// levers should always start to left.
short handle_lever(location w)
{
	if (FCD(1020,0) == 1)
		return 0;
	play_sound(94);
	switch_lever(w);
	return 1;
}

void switch_lever(location w)
{
	alter_space(w.x,w.y,scenario.ter_types[univ.town->terrain(w.x,w.y)].trans_to_what);
}

