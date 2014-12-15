
#include <iostream>

//#include "item.h"

#include "boe.global.h"

#include <array>

#include "classes.h"

#include "boe.fileio.h"
#include "boe.graphics.h"
#include "boe.graphutil.h"
#include "boe.newgraph.h"
#include "boe.specials.h"
#include "boe.itemdata.h"
#include "boe.infodlg.h"
#include "boe.items.h"
#include <cstring>
#include "boe.party.h"
#include "boe.monster.h"
#include "boe.town.h"
#include "boe.combat.h"
#include "boe.locutils.h"
#include "boe.text.h"
#include "soundtool.h"
#include "boe.main.h"
#include "graphtool.h"
#include "mathutil.h"
#include "dlogutil.h"
#include "winutil.h"
#include "fileio.h"
#include "boe.menus.h"
#include "restypes.hpp"
#include <array>
#include <boost/lexical_cast.hpp>
#include "button.h"
#include "spell.hpp"

extern short skill_bonus[21];

// TODO: Use magic-names.txt instead of these arrays
const char *mage_s_name[62] = {
	"Light","Spark","Minor Haste","Strength","Scare",
	"Flame Cloud","Identify","Scry Monster","Goo","True Sight",
	"Minor Poison","Flame","Slow","Dumbfound","Envenom",
	"Stinking Cloud","Summon Beast","Conflagration","Dispel Field","Sleep Cloud",
	"Unlock","Haste","Fireball","Long Light","Fear",
	"Wall of Force","Weak Summoning","Flame Arrows","Web","Resist Magic",
	"Poison","Ice Bolt","Slow Group","Magic Map",
	"Capture Soul","Simulacrum","Venom Arrows","Wall of Ice",
	"Stealth","Major Haste","Fire Storm","D. Barrier",
	"Fire Barrier","Summoning","Shockstorm","Spray Fields",
	"Major Poison","Group Fear","Kill","Paralysis",
	"Daemon","Antimagic Cloud","MindDuel","Flight",
	"Shockwave","M. Blessing","Mass Paralysis","Protection",
	"Major Summon","Force Barrier","Quickfire","Death Arrows"};
const char *priest_s_name[62] = {
	"Minor Bless","Minor Heal","Weaken Poison","Turn Undead","Location",
	"Sanctuary","Symbiosis","Minor Manna","Ritual - Sanctify","Stumble",
	"Bless","Cure Poison","Curse","Light","Wound",
	"Summon Spirit","Move Mountains","Charm Foe","Disease","Awaken",
	"Heal","Light Heal All","Holy Scourge","Detect Life","Cure Paralysis",
	"Manna","Forcefield","Cure Disease","Restore Mind","Smite",
	"Cure Party","Curse All","Dispel Undead","Remove Curse",
	"Sticks to Snakes","Martyr's Shield","Cleanse","Firewalk",
	"Bless Party","Major Heal","Raise Dead","Flamestrike",
	"Mass Sanctuary","Summon Host","Shatter","Dispel Fields",
	"Heal All","Revive","Hyperactivity","Destone",
	"Guardian","Mass Charm","Protective Circle","Pestilence",
	"Revive All","Ravage Spirit","Resurrect","Divine Thud",
	"Avatar","Wall of Blades","Word of Recall","Major Cleansing"};
extern const char* alch_names[20];
const char *alch_names_short[20] = {
	"Weak Curing Potion","Weak Healing Potion","Weak Poison",
	"Weak Speed Potion","Medium Poison",
	"Medium Heal Potion","Strong Curing","Medium Speed Potion",
	"Graymold Salve","Weak Energy Potion",
	"Potion of Clarity","Strong Poison","Strong Heal Potion","Killer Poison",
	"Resurrection Bal","Medium Energy Ptn.","Knowledge Brew"	,
	"Strong Strength","Bliss","Strong Power"
};
bool get_mage[30] = {1,1,1,1,1,1,0,1,1,0, 1,1,1,1,1,1,0,0,1,1, 1,1,1,1,1,0,0,0,1,1};
bool get_priest[30] = {1,1,1,1,1,1,0,0,0,1, 1,1,1,1,1,0,0,0,1,1, 1,0,1,1,0,0,0,1,0,0};
short combat_percent[20] = {
	150,120,100,90,80,80,80,70,70,70,
	70,70,67,62,57,52,47,42,40,40};

short who_cast,which_pc_displayed;
eSpell town_spell;
extern bool spell_freebie;
bool spell_button_active[90];
extern cItemRec start_items[6];

char empty_string[256] = "                                           ";

//extern stored_town_maps_type town_maps;
extern bool fast_bang;
extern bool flushingInput;
//extern party_record_type	party;
extern short stat_window,current_pc;
extern eGameMode overall_mode;
//extern current_town_type	univ.town;
//extern big_tr_type t_d;
//extern unsigned char out[96][96],out_e[96][96];
extern fs::path progDir;
extern location center;
extern sf::RenderWindow mainPtr;
extern bool spell_forced,save_maps,suppress_stat_screen,boom_anim_active;
//extern stored_items_list_type stored_items[3];
//extern CursHandle sword_curs;
//extern setup_save_type setup_save;
extern eSpell store_mage, store_priest;
//extern cOutdoors outdoors[2][2];
extern short store_mage_lev, store_priest_lev;
extern short store_spell_target,pc_casting,stat_screen_mode;
extern effect_pat_type null_pat,single,t,square,rad2,rad3;
extern effect_pat_type current_pat;
extern short current_spell_range;
extern short hit_chance[21],combat_active_pc;//,pc_moves[6];
extern short boom_gr[8];
//extern	unsigned char beasts[5];
//extern	unsigned char m1[20];
//extern	unsigned char m2[16];
//extern	unsigned char m3[16];
//extern stored_town_maps_type maps;
//extern stored_outdoor_maps_type o_maps;
extern short current_ground;
extern short monst_marked_damage[60];
extern location golem_m_locs[16];
//extern town_item_list t_i;
extern cScenario scenario;
extern cUniverse univ;
//extern piles_of_stuff_dumping_type *data_store;
extern sf::Texture pc_gworld;
char c_line[60];

// Variables for spell selection
short on_which_spell_page = 0;
short store_last_cast_mage = 6,store_last_cast_priest = 6;
short buttons_on[38] = {1,1,1,1,1,1,1,1,0,0,1,1,1,1,1,1,1,1,0,0,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0};
// buttons_on determines which buttons can be hit when on the second spell page
short spell_index[38] = {38,39,40,41,42,43,44,45,90,90,46,47,48,49,50,51,52,53,90,90,
	54,55,56,57,58,59,60,61,90,90, 90,90,90,90,90,90,90,90};
// Says which buttons hit which spells on second spell page, 90 means no button
bool can_choose_caster;

// Dialog vars
short store_graphic_pc_num ;
short store_graphic_mode ;
short store_pc_graphic;

//mode; // 0 - prefab 1 - regular 2 - debug
// Note: mode 1 is never used
void init_party(short mode)
{
	// TODO: Remove in favour of cParty constructor.
	short i,j,k,l;
	
	cVehicle null_boat;// = {{0,0},{0,0},{0,0},200,false};
	cVehicle null_horse;// = {{0,0},{0,0},{0,0},200,false};
	
	univ.party.age = 0;
	univ.party.gold = 200;
	univ.party.food = 100;
	for(i = 0; i < 310; i++)
		for(j = 0; j < 10; j++)
			PSD[i][j] = 0;
	univ.party.light_level = 0;
	univ.party.outdoor_corner.x = 7;
	univ.party.outdoor_corner.y = 8;
	univ.party.i_w_c.x = 1;
	univ.party.i_w_c.y = 1;
	univ.party.loc_in_sec.x = 36;
	univ.party.loc_in_sec.y = 36;
	univ.party.p_loc.x = 84;
	univ.party.p_loc.y = 84;
	for(i = 0; i < 30; i++)
		univ.party.boats[i] = null_boat;
	for(i = 0; i < 30; i++)
		univ.party.horses[i] = null_horse;
	univ.party.in_boat = -1;
	univ.party.in_horse = -1;
	for(i = 0; i < 4; i++)
		univ.party.creature_save[i].which_town = 200;
	for(i = 0; i < 10; i++)
		univ.party.out_c[i].exists = false;
	for(i = 0; i < 5; i++)
		for(j = 0; j < 10; j++)
			univ.party.magic_store_items[i][j].variety = eItemType::NO_ITEM;
	for(i = 0; i < 4; i++)
		univ.party.imprisoned_monst[i] = 0;
	for(i = 0; i < 256; i++)
		univ.party.m_seen[i] = univ.party.m_noted[i] = 0;
//	for(i = 0; i < 50; i++)
//		univ.party.journal_str[i] = -1;
//	for(i = 0; i < 140; i++)
//		for(j = 0; j < 2; j++)
//			univ.party.special_notes_str[i][j] = 0;
//	for(i = 0; i < 120; i++)
//		univ.party.talk_save[i].personality = -1;
	univ.party.journal.clear();
	univ.party.special_notes.clear();
	univ.party.talk_save.clear();
	
	univ.party.total_m_killed = 0;
	univ.party.total_dam_done = 0;
	univ.party.total_xp_gained = 0;
	univ.party.total_dam_taken = 0;
	univ.party.direction = 0;
	univ.party.at_which_save_slot = 0;
	for(i = 0; i < 20; i++)
		univ.party.alchemy[i] = 0;
	for(i = 0; i < 200; i++)
		univ.party.can_find_town[i] = 0;
	for(i = 0; i < 20; i++)
		univ.party.key_times[i] = 30000;
	univ.party.party_event_timers.clear();
	for(i = 0; i < 50; i++)
		univ.party.spec_items[i] = 0;
	for(i = 0; i < 200; i++)
		univ.party.m_killed[i] = 0;
	univ.party.scen_name = "";
	
	for(i = 0; i < 200; i++)
		for(j = 0; j < 8; j++)
			univ.party.item_taken[i][j] = 0;
	
	// Zero out campaign flags and pointers
	univ.party.campaign_flags.clear();
	univ.party.pointers.clear();
	
	
	refresh_store_items();
	
	for(i = 0; i < 6; i++) {
		switch(mode){
			case 0:
				univ.party[i] = cPlayer('dflt',i);
				break;
			case 1:
				univ.party[i] = cPlayer();
				break;
			case 2:
				univ.party[i] = cPlayer('dbug',i);
				break;
		}
	}
	
	for(i = 0; i < 96; i++)
		for(j = 0; j < 96; j++)
			univ.out.out_e[i][j] = 0;
	
	
	for(i = 0; i < 3;i++)
		for(j = 0; j < NUM_TOWN_ITEMS; j++) {
			univ.party.stored_items[i][j] = cItemRec();
		}
	
	for(i = 0; i < 200; i++)
		for(j = 0; j < 8; j++)
			for(k = 0; k < 64; k++)
				univ.town_maps[i][j][k] = 0;
	
	for(i = 0; i < 100; i++)
		for(k = 0; k < 6; k++)
			for(l = 0; l < 48; l++)
				univ.out_maps[i][k][l] = 0;
	
	// Default is save maps
	PSD[SDF_NO_MAPS] = 0;
	save_maps = true;
	
	
	// NOT DEBUG
	build_outdoors();
	
}

// This is only called after a scenario is loaded and the party is put into it.
// Until that time, the party scen vals are uninited
// Then, it inits the party properly for starting the scenario based
// on the loaded scenario
void init_party_scen_data()
{
	short i,j,k,l;
	bool stored_item = false;
	short store_help;
	
	univ.party.age = 0;
	store_help = PSD[SDF_NO_INSTANT_HELP];
	for(i = 0; i < 310; i++)
		for(j = 0; j < 10; j++)
			PSD[i][j] = 0;
	PSD[SDF_NO_INSTANT_HELP] = store_help;
	univ.party.light_level = 0;
	univ.party.outdoor_corner.x = scenario.out_sec_start.x;
	univ.party.outdoor_corner.y = scenario.out_sec_start.y;
	univ.party.i_w_c.x = 0;
	univ.party.i_w_c.y = 0;
	univ.party.loc_in_sec.x = scenario.out_start.x;
	univ.party.loc_in_sec.y = scenario.out_start.y;
	univ.party.p_loc.x = scenario.out_start.x;
	univ.party.p_loc.y = scenario.out_start.y;
	for(i = 0; i < 30; i++)
		univ.party.boats[i] = scenario.boats[i];
	for(i = 0; i < 30; i++)
		univ.party.horses[i] = scenario.horses[i];
	for(i = 0; i < 30; i++) {
		if((scenario.boats[i].which_town >= 0) && (scenario.boats[i].loc.x >= 0)) {
			if(!univ.party.boats[i].exists) {
				univ.party.boats[i] = scenario.boats[i];
				univ.party.boats[i].exists = true;
			}
		}
		if((scenario.horses[i].which_town >= 0) && (scenario.horses[i].loc.x >= 0)) {
			if(!univ.party.horses[i].exists) {
				univ.party.horses[i] = scenario.horses[i];
				univ.party.horses[i].exists = true;
			}
		}
	}
	univ.party.in_boat = -1;
	univ.party.in_horse = -1;
	for(i = 0; i < 4; i++)
		univ.party.creature_save[i].which_town = 200;
	for(i = 0; i < 10; i++)
		univ.party.out_c[i].exists = false;
	for(i = 0; i < 5; i++)
		for(j = 0; j < 10; j++)
			univ.party.magic_store_items[i][j].variety = eItemType::NO_ITEM;
	for(i = 0; i < 4; i++)
		univ.party.imprisoned_monst[i] = 0;
	for(i = 0; i < 256; i++)
		univ.party.m_seen[i] = univ.party.m_noted[i] = 0;
//	for(i = 0; i < 50; i++)
//		univ.party.journal_str[i] = -1;
//	for(i = 0; i < 140; i++)
//		for(j = 0; j < 2; j++)
//			univ.party.special_notes_str[i][j] = 0;
//	for(i = 0; i < 120; i++)
//		univ.party.talk_save[i].personality = -1;
	// TODO: The journal at least should persist across scenarios; the special and talk notes, maybe, maybe not
	univ.party.special_notes.clear();
	univ.party.talk_save.clear();
	
	univ.party.direction = 0;
	univ.party.at_which_save_slot = 0;
	for(i = 0; i < 200; i++)
		univ.party.can_find_town[i] = 1 - scenario.town_hidden[i];
	for(i = 0; i < 20; i++)
	 	univ.party.key_times[i] = 30000;
	univ.party.party_event_timers.clear();
	for(i = 0; i < 50; i++)
		univ.party.spec_items[i] = (scenario.special_items[i].flags >= 10) ? 1 : 0;
	
	for(i = 0; i < 200; i++)
		univ.party.m_killed[i] = 0;
	
	for(i = 0; i < 200; i++)
		for(j = 0; j < 8; j++)
			univ.party.item_taken[i][j] = 0;
	
	
	refresh_store_items();
	
	for(i = 0; i < 96; i++)
		for(j = 0; j < 96; j++)
			univ.out.out_e[i][j] = 0;
	
	for(i = 0; i < 3;i++)
		for(j = 0; j < NUM_TOWN_ITEMS; j++)
			if(univ.party.stored_items[i][j].variety != eItemType::NO_ITEM)
				stored_item = true;
	if(stored_item)
		if(cChoiceDlog("keep-stored-items.xml", {"yes", "no"}).show() == "yes") {
			std::vector<cItemRec*> saved_item_refs;
			for(i = 0; i < 3;i++)
				for(j = 0; j < NUM_TOWN_ITEMS; j++)
					if(univ.party.stored_items[i][j].variety != eItemType::NO_ITEM)
						saved_item_refs.push_back(&univ.party.stored_items[i][j]);
			short pc = 0;
			while(univ.party[pc].main_status != eMainStatus::ALIVE && pc < 6) pc++;
			show_get_items("Choose stored items to keep:", saved_item_refs, pc, true);
		}
	for(i = 0; i < 3;i++)
		for(j = 0; j < NUM_TOWN_ITEMS; j++) {
			univ.party.stored_items[i][j] = cItemRec();
		}
	
	for(i = 0; i < 200; i++)
		for(j = 0; j < 8; j++)
			for(k = 0; k < 64; k++)
				univ.town_maps[i][j][k] = 0;
	
	for(i = 0; i < 100; i++)
		for(k = 0; k < 6; k++)
			for(l = 0; l < 48; l++)
				univ.out_maps[i][k][l] = 0;
	
}

// When the party is placed into a scen from the startinbg screen, this is called to put the game into game
// mode and load in the scen and init the party info
// party record already contains scen name
void put_party_in_scen(std::string scen_name)
{
	short i,j;
	std::array<std::string, 6> strs;
	std::array<short, 3> buttons = {-1,-1,-1};
	bool item_took = false;
	
	for(j = 0; j < 6; j++) {
		univ.party[i].status.clear();
		if(isSplit(univ.party[j].main_status))
			univ.party[j].main_status -= eMainStatus::SPLIT;
		univ.party[j].cur_health = univ.party[j].max_health;
 		univ.party[j].cur_sp = univ.party[j].max_sp;
	}
	for(j = 0; j < 6; j++)
	 	for(i = 23; i >= 0; i--) {
			univ.party[j].items[i].special_class = 0;
			// TODO: Don't take items just because they have a special graphic
			if(univ.party[j].items[i].graphic_num >= 150) {
				take_item(j,i + 30); // strip away special items
				item_took = true;
			}
			if(univ.party[j].items[i].ability == eItemAbil::SUMMONING) {
				take_item(j,i + 30); // strip away summoning items
				item_took = true;
			}
			if(univ.party[j].items[i].ability == eItemAbil::MASS_SUMMONING) {
				take_item(j,i + 30); // strip away summoning items
				item_took = true;
			}
		}
	if(item_took)
		cChoiceDlog("removed-special-items.xml").show();
	univ.party.age = 0;
	for(i = 0; i < 200; i++)
		univ.party.m_killed[i] = 0;
	univ.party.party_event_timers.clear();
	
	fs::path path;
	path = progDir/"Blades of Exile Scenarios";
	path /= scen_name;
	std::cout<<"Searching for scenario at:\n"<<path<<'\n';
	if(!load_scenario(path))
		return;
	
	init_party_scen_data();
	univ.party.scen_name = scen_name;
	
	// if at this point, startup must be over, so make this call to make sure we're ready,
	// graphics wise
	end_startup();
	
	load_outdoors(loc(univ.party.outdoor_corner.x + 1,univ.party.outdoor_corner.y + 1),univ.out.outdoors[1][1]);
	load_outdoors(loc(univ.party.outdoor_corner.x,univ.party.outdoor_corner.y + 1),univ.out.outdoors[0][1]);
	load_outdoors(loc(univ.party.outdoor_corner.x + 1,univ.party.outdoor_corner.y),univ.out.outdoors[1][0]);
	load_outdoors(loc(univ.party.outdoor_corner.x,univ.party.outdoor_corner.y),univ.out.outdoors[0][0]);
	stat_screen_mode = 0;
	build_outdoors();
	erase_out_specials();
	
	current_pc = first_active_pc();
	force_town_enter(scenario.which_town_start,scenario.where_start);
	start_town_mode(scenario.which_town_start,9);
	center = scenario.where_start;
	update_explored(scenario.where_start);
	overall_mode = MODE_TOWN;
	redraw_screen(REFRESH_ALL);
	mainPtr.display(); // TODO: Maybe display() should be called in redraw_screen()?
	set_stat_window(0);
	adjust_spell_menus();
	adjust_monst_menu();
	
	// Throw up intro dialog
	buttons[0] = 1;
	for(j = 0; j < 6; j++)
		if(strlen(	scenario.scen_strs(4 + j)) > 0) {
			for(i = 0; i < 6; i++)
				strs[i] = scenario.scen_strs(4 + i);
			custom_choice_dialog(strs,scenario.intro_pic,PIC_SCEN,buttons) ;
			j = 6;
		}
	give_help(1,2);
	
	// this is kludgy, put here to prevent problems
	for(i = 0; i < 50; i++)
		univ.party.spec_items[i] = (scenario.special_items[i].flags >= 10) ? 1 : 0;
	
	// Compatibility flags
	if(scenario.format.prog_make_ver[0] < 2){
		PSD[SDF_RESURRECT_NO_BALM] = 1;
		PSD[SDF_NO_BOAT_SPECIALS] = 1;
		PSD[SDF_TIMERS_DURING_REST] = 0;
	} else {
		PSD[SDF_RESURRECT_NO_BALM] = 0;
		PSD[SDF_NO_BOAT_SPECIALS] = 0;
		PSD[SDF_TIMERS_DURING_REST] = 1;
	}
}

bool create_pc(short spot,cDialog* parent)
//spot; // if spot is 6, find one
{
	bool still_ok = true;
	
	if(spot == 6) {
		for(spot = 0; spot < 6; spot++)
			if(univ.party[spot].main_status == eMainStatus::ABSENT)
				break;
	}
	if(spot == 6)
		return false;
	
	univ.party[spot] = cPlayer();
	
	pick_race_abil(&univ.party[spot],0);
	
	// TODO: Not sure if these initial draws are needed
//	if(parent != NULL)
//		cd_initial_draw(989);
	
	still_ok = spend_xp(spot,0,parent);
	if(!still_ok)
		return false;
	univ.party[spot].cur_health = univ.party[spot].max_health;
	univ.party[spot].cur_sp = univ.party[spot].max_sp;
//	if(parent != NULL)
//		cd_initial_draw(989);
	
	pick_pc_graphic(spot,0,parent);
	
//	if(parent != NULL)
//		cd_initial_draw(989);
	pick_pc_name(spot,parent);
	
	univ.party[spot].main_status = eMainStatus::ALIVE;
	
	if(overall_mode != MODE_STARTUP) {
		// TODO: start_items will need to be extended for Vahnatai race
		int raceCode = (int) univ.party[spot].race;
		univ.party[spot].items[0] = start_items[raceCode * 2];
		univ.party[spot].equip[0] = true;
		univ.party[spot].items[1] = start_items[raceCode * 2 + 1];
		univ.party[spot].equip[1] = true;
		
		// Do stat adjs for selected race.
		if(univ.party[spot].race == eRace::NEPHIL)
			univ.party[spot].skills[eSkill::DEXTERITY] += 2;
		if(univ.party[spot].race == eRace::SLITH) {
			univ.party[spot].skills[eSkill::STRENGTH] += 2;
			univ.party[spot].skills[eSkill::INTELLIGENCE] += 1;
		}
		// TODO: Vahnatai
		// TODO: Why is this code in two different places?
		univ.party[spot].max_sp += univ.party[spot].skills[eSkill::MAGE_SPELLS] * 3 + univ.party[spot].skills[eSkill::PRIEST_SPELLS] * 3;
		univ.party[spot].cur_sp = univ.party[spot].max_sp;
	}
	
	return true;
}

bool take_sp(short pc_num,short amt)
{
	if(univ.party[pc_num].cur_sp < amt)
		return false;
	univ.party[pc_num].cur_sp -= amt;
	return true;
}




void heal_pc(short pc_num,short amt)
{
	if(univ.party[pc_num].cur_health > univ.party[pc_num].max_health)
		return;
	if(univ.party[pc_num].main_status != eMainStatus::ALIVE)
		return;
	univ.party[pc_num].cur_health += amt;
	if(univ.party[pc_num].cur_health > univ.party[pc_num].max_health)
		univ.party[pc_num].cur_health = univ.party[pc_num].max_health;
	
}

void heal_party(short amt)
{
	short i;
	
	for(i = 0; i < 6; i++)
		if(univ.party[i].main_status == eMainStatus::ALIVE)
			heal_pc(i,amt);
}

void cure_pc(short pc_num,short amt)
{
	if(univ.party[pc_num].main_status != eMainStatus::ALIVE)
		return;
	if(univ.party[pc_num].status[eStatus::POISON] <= amt)
		univ.party[pc_num].status[eStatus::POISON] = 0;
	else univ.party[pc_num].status[eStatus::POISON] -= amt;
	one_sound(51);
}

void cure_party(short amt)
{
	short i;
	
	for(i = 0; i < 6; i++)
		if(univ.party[i].main_status == eMainStatus::ALIVE)
			cure_pc(i,amt);
	
}

// if how_much < 0, bless
void curse_pc(short which_pc,short how_much)
{
	if(univ.party[which_pc].main_status != eMainStatus::ALIVE)
		return;
	if(univ.party[which_pc].main_status == eMainStatus::ALIVE) {
		univ.party[which_pc].status[eStatus::BLESS_CURSE] = minmax(-8,8,univ.party[which_pc].status[eStatus::BLESS_CURSE] - how_much);
		if(how_much < 0)
			sprintf ((char *) c_line, "  %s blessed.",(char *) univ.party[which_pc].name.c_str());
		else sprintf ((char *) c_line, "  %s cursed.",(char *) univ.party[which_pc].name.c_str());
		add_string_to_buf((char *) c_line);
	}
	put_pc_screen();
	if(how_much < 0)
		give_help(59,0);
}

void dumbfound_pc(short which_pc,short how_much)
{
	short r1;
	
	if(univ.party[which_pc].main_status != eMainStatus::ALIVE)
		return;
	r1 = get_ran(1,0,90);
	if(pc_has_abil_equip(which_pc,eItemAbil::WILL) < 24) {
		add_string_to_buf("  Ring of Will glows.");
		r1 -= 10;
	}
	if(r1 < univ.party[which_pc].level)
		how_much -= 2;
	if(how_much <= 0) {
		sprintf ((char *) c_line, "  %s saved.",(char *) univ.party[which_pc].name.c_str());
		add_string_to_buf((char *) c_line);
		return;
	}
	if(univ.party[which_pc].main_status == eMainStatus::ALIVE) {
		univ.party[which_pc].status[eStatus::DUMB] = min(univ.party[which_pc].status[eStatus::DUMB] + how_much,8);
		sprintf ((char *) c_line, "  %s dumbfounded.",(char *) univ.party[which_pc].name.c_str());
		add_string_to_buf((char *) c_line);
	}
	one_sound(67);
	put_pc_screen();
	adjust_spell_menus();
	give_help(28,0);
}
void disease_pc(short which_pc,short how_much)
{
	short r1,level;
	
	if(univ.party[which_pc].main_status != eMainStatus::ALIVE)
		return;
	r1 = get_ran(1,1,100);
	if(r1 < univ.party[which_pc].level * 2)
		how_much -= 2;
	if(how_much <= 0) {
		sprintf ((char *) c_line, "  %s saved.",(char *) univ.party[which_pc].name.c_str());
		add_string_to_buf((char *) c_line);
		return;
	}
	if((level = get_prot_level(which_pc,eItemAbil::PROTECT_FROM_DISEASE)) > 0)
		how_much -= level / 2;
	if(univ.party[which_pc].traits[eTrait::FRAIL] && how_much > 1)
		how_much++;
	if(univ.party[which_pc].traits[eTrait::FRAIL] && how_much == 1 && get_ran(1,0,1) == 0)
		how_much++;
	if(univ.party[which_pc].main_status == eMainStatus::ALIVE) {
		univ.party[which_pc].status[eStatus::DISEASE] = min(univ.party[which_pc].status[eStatus::DISEASE] + how_much,8);
		sprintf ((char *) c_line, "  %s diseased.",(char *) univ.party[which_pc].name.c_str());
		add_string_to_buf((char *) c_line);
	}
	one_sound(66);
	put_pc_screen();
	give_help(29,0);
}

void sleep_pc(short which_pc,short how_much,eStatus what_type,short adjust)
// higher adjust, less chance of saving
{
	short r1,level;
	if(univ.party[which_pc].main_status != eMainStatus::ALIVE)
		return;
	if(how_much == 0)
		return;
	// TODO: Uh, what if an invalid status is passed in?
	// --> Currently, you'd get that status effect, but with a "paralyzed" message, sound, and quick-help
	if(what_type == eStatus::ASLEEP || what_type == eStatus::PARALYZED) { ////
		if((level = get_prot_level(which_pc,eItemAbil::WILL)) > 0)
			how_much -= level / 2;
		if((level = get_prot_level(which_pc,eItemAbil::FREE_ACTION)) > 0)
			how_much -= (what_type == eStatus::ASLEEP) ? level : level * 300;
		
	}
	
	r1 = get_ran(1,1,100) + adjust;
	if(r1 < 30 + univ.party[which_pc].level * 2)
		how_much = -1;
	if(what_type == eStatus::ASLEEP && (univ.party[which_pc].traits[eTrait::HIGHLY_ALERT] || univ.party[which_pc].status[eStatus::ASLEEP] < 0))
		how_much = -1;
	if(how_much <= 0) {
		sprintf ((char *) c_line, "  %s resisted.",(char *) univ.party[which_pc].name.c_str());
		add_string_to_buf((char *) c_line);
		return;
	}
	if(univ.party[which_pc].main_status == eMainStatus::ALIVE) {
		univ.party[which_pc].status[what_type] = how_much;
		if(what_type == eStatus::ASLEEP)
			sprintf ((char *) c_line, "  %s falls asleep.",(char *) univ.party[which_pc].name.c_str());
		else sprintf ((char *) c_line, "  %s paralyzed.",(char *) univ.party[which_pc].name.c_str());
		if(what_type == eStatus::ASLEEP)
			play_sound(96);
		else play_sound(90);
		add_string_to_buf((char *) c_line);
		univ.party[which_pc].ap = 0;
	}
	put_pc_screen();
	if(what_type == eStatus::ASLEEP)
		give_help(30,0);
	else give_help(32,0);
	
}

// if how_much < 0, haste
void slow_pc(short which_pc,short how_much)////
{
	if(univ.party[which_pc].main_status != eMainStatus::ALIVE)
		return;
	if(univ.party[which_pc].main_status == eMainStatus::ALIVE) {
		
		univ.party[which_pc].status[eStatus::HASTE_SLOW] = minmax(-8,8,univ.party[which_pc].status[eStatus::HASTE_SLOW] - how_much);
		if(how_much < 0)
			sprintf ((char *) c_line, "  %s hasted.",(char *) univ.party[which_pc].name.c_str());
		else sprintf ((char *) c_line, "  %s slowed.",(char *) univ.party[which_pc].name.c_str());
		add_string_to_buf((char *) c_line);
	}
	put_pc_screen();
	if(how_much < 0)
		give_help(35,0);
}

void web_pc(short which_pc,short how_much)
{
	if(univ.party[which_pc].main_status != eMainStatus::ALIVE)
		return;
	if(univ.party[which_pc].main_status == eMainStatus::ALIVE) {
		univ.party[which_pc].status[eStatus::WEBS] = min(univ.party[which_pc].status[eStatus::WEBS] + how_much,8);
		sprintf ((char *) c_line, "  %s webbed.",(char *) univ.party[which_pc].name.c_str());
		add_string_to_buf((char *) c_line);
		one_sound(17);
	}
	put_pc_screen();
	give_help(31,0);
}

void acid_pc(short which_pc,short how_much)
{
	if(univ.party[which_pc].main_status != eMainStatus::ALIVE)
		return;
	if(pc_has_abil_equip(which_pc,eItemAbil::ACID_PROTECTION) < 24) {
		sprintf ((char *) c_line, "  %s resists acid.",(char *) univ.party[which_pc].name.c_str());
		add_string_to_buf((char *) c_line);
		return;
	}
	if(univ.party[which_pc].main_status == eMainStatus::ALIVE) {
		univ.party[which_pc].status[eStatus::ACID] += how_much;
		sprintf ((char *) c_line, "  %s covered with acid!",(char *) univ.party[which_pc].name.c_str());
		add_string_to_buf((char *) c_line);
		one_sound(42);
	}
	put_pc_screen();
}

void increase_light(short amt)
{
	short i;
	location where;
	
	univ.party.light_level += amt;
	if(univ.party.light_level < 0)
		univ.party.light_level = 0;
	if(is_combat()) {
		for(i = 0; i < 6; i++)
			if(univ.party[i].main_status == eMainStatus::ALIVE) {
				update_explored(univ.party[i].combat_pos);
			}
	}
	else {
		where = get_cur_loc();
		update_explored(where);
	}
	put_pc_screen();
}

void restore_sp_pc(short pc_num,short amt)
{
	if(univ.party[pc_num].cur_sp > univ.party[pc_num].max_sp)
		return;
	univ.party[pc_num].cur_sp += amt;
	if(univ.party[pc_num].cur_sp > univ.party[pc_num].max_sp)
		univ.party[pc_num].cur_sp = univ.party[pc_num].max_sp;
}

void restore_sp_party(short amt)
{
	short i;
	
	for(i = 0; i < 6; i++)
		if(univ.party[i].main_status == eMainStatus::ALIVE)
			restore_sp_pc(i,amt);
}

void award_party_xp(short amt)
{
	short i;
	
	for(i = 0; i < 6; i++)
		if(univ.party[i].main_status == eMainStatus::ALIVE)
			award_xp(i,amt);
}

void award_xp(short pc_num,short amt)
{
	short adjust,add_hp;
	short xp_percent[30] = {
		150,120,100,90,80,70,60,50,50,50,
		45,40,40,40,40,35,30,25,23,20,
		15,15,15,15,15,15,15,15,15,15};
	if(univ.party[pc_num].level > 49) {
		univ.party[pc_num].level = 50;
		return;
	}
	if(amt > 200) { // debug
		beep();
		ASB("Oops! Too much xp!");
		ASB("Report this!");
		return;
	}
	if(amt < 0) { // debug
		beep();
		ASB("Oops! Negative xp!");
		ASB("Report this!");
		return;
	}
	if(univ.party[pc_num].main_status != eMainStatus::ALIVE)
		return;
	
	if(univ.party[pc_num].level >= 40)
		adjust = 15;
	else adjust = xp_percent[univ.party[pc_num].level / 2];
	
	if((amt > 0) && (univ.party[pc_num].level > 7)) {
		if(get_ran(1,1,100) < xp_percent[univ.party[pc_num].level / 2])
			amt--;
	}
	if(amt <= 0)
		return;
	
	
//	univ.party[pc_num].experience += (max(((amt * adjust) / 100), 0) * univ.party[pc_num].exp_adj) / 100;
//	univ.party.total_xp_gained += (max(((amt * adjust) / 100), 0) * univ.party[pc_num].exp_adj) / 100;
	univ.party[pc_num].experience += (max(((amt * adjust) / 100), 0) * 100) / 100;
	univ.party.total_xp_gained += (max(((amt * adjust) / 100), 0) * 100) / 100;
	
	
	
//	if(univ.party[pc_num].experience < 0) {
//		SysBeep(50); SysBeep(50);
//		ASB("Oops! Xp became negative somehow!");
//		ASB("Report this!");
//		univ.party[pc_num].experience = univ.party[pc_num].level * (univ.party[pc_num].get_tnl()) - 1;
//		return;
//		}
	if(univ.party[pc_num].experience > 15000) {
		univ.party[pc_num].experience = 15000;
		return;
	}
	
	while(univ.party[pc_num].experience >= (univ.party[pc_num].level * (univ.party[pc_num].get_tnl()))) {
		play_sound(7);
		univ.party[pc_num].level++;
		sprintf ((char *) c_line, "  %s is level %d!  ",(char *) univ.party[pc_num].name.c_str(),univ.party[pc_num].level);
		add_string_to_buf((char *) c_line);
		univ.party[pc_num].skill_pts += (univ.party[pc_num].level < 20) ? 5 : 4;
		add_hp = (univ.party[pc_num].level < 26) ? get_ran(1,2,6) + skill_bonus[univ.party[pc_num].skills[eSkill::STRENGTH]]
			: max (skill_bonus[univ.party[pc_num].skills[eSkill::STRENGTH]],0);
		if(add_hp < 0)
			add_hp = 0;
		univ.party[pc_num].max_health += add_hp;
		if(univ.party[pc_num].max_health > 250)
			univ.party[pc_num].max_health = 250;
		univ.party[pc_num].cur_health += add_hp;
		if(univ.party[pc_num].cur_health > 250)
			univ.party[pc_num].cur_health = 250;
		put_pc_screen();
		
	}
}

void drain_pc(short which_pc,short how_much)
{
	if(univ.party[which_pc].main_status == eMainStatus::ALIVE) {
		univ.party[which_pc].experience = max(univ.party[which_pc].experience - how_much,0);
		sprintf ((char *) c_line, "  %s drained.",(char *) univ.party[which_pc].name.c_str());
		add_string_to_buf((char *) c_line);
	}
}

static short check_party_stat_get(short pc, eSkill which_stat) {
	switch(which_stat) {
		case eSkill::MAX_HP:
			return univ.party[pc].max_health;
		case eSkill::MAX_SP:
			return univ.party[pc].max_sp;
		case eSkill::CUR_HP:
			return univ.party[pc].cur_health;
		case eSkill::CUR_SP:
			return univ.party[pc].cur_sp;
		case eSkill::CUR_XP:
			return univ.party[pc].experience;
		case eSkill::CUR_SKILL:
			return univ.party[pc].skill_pts;
		case eSkill::CUR_LEVEL:
			return univ.party[pc].level;
		default:
			return univ.party[pc].skills[which_stat];
	}
	return 0;
}

// mode: 0 = total, 1 = mean, 2 = min, 3 = max, 10+i = just PC i
short check_party_stat(eSkill which_stat, short mode) {
	if(mode >= 10) return check_party_stat_get(mode - 10, which_stat);
	
	short total = mode == 2 ? std::numeric_limits<short>::max() : 0, num_pcs = 0;
	
	for(short i = 0; i < 6; i++)
		if(univ.party[i].main_status == eMainStatus::ALIVE) {
			num_pcs++;
			if(mode < 2)
				total += check_party_stat_get(i,which_stat);
			else if(mode == 2)
				total = max(check_party_stat_get(i,which_stat), total);
			else if(mode == 3)
				total = min(check_party_stat_get(i,which_stat), total);
		}
	
	if(mode == 1 && num_pcs > 0)
		total /= num_pcs;
	
	return total;
}


bool poison_weapon( short pc_num, short how_much,short safe)
//short safe; // 1 - always succeeds
{
	short i,weap = 24,p_level,r1;
	short p_chance[21] = {
		40,72,81,85,88,89,90,
		91,92,93,94,94,95,95,96,97,98,100,100,100,100};
	
	for(i = 0; i < 24; i++)
		if((univ.party[pc_num].equip[i]) && (is_weapon(pc_num,i))) {
			weap = i;
			i = 30;
		}
	if(weap == 24) {
		add_string_to_buf("  No weapon equipped.       ");
		return false;
	}
	else {
		p_level = how_much;
		add_string_to_buf("  You poison your weapon.       ");
		r1 = get_ran(1,1,100);
		// Nimble?
		if(univ.party[pc_num].traits[eTrait::NIMBLE])
			r1 -= 6;
		if((r1 > p_chance[univ.party[pc_num].skills[eSkill::POISON]]) && (safe == 0)) {
			add_string_to_buf("  Poison put on badly.         ");
			p_level = p_level / 2;
			r1 = get_ran(1,1,100);
			if(r1 > p_chance[univ.party[pc_num].skills[eSkill::POISON]] + 10) {
				add_string_to_buf("  You nick yourself.          ");
				univ.party[pc_num].status[eStatus::POISON] += p_level;
			}
		}
		if(safe != 1)
			play_sound(55);
		univ.party[pc_num].weap_poisoned = weap;
		max (univ.party[pc_num].status[eStatus::POISON], p_level);
		
		return true;
	}
}

bool is_weapon(short pc_num,short item)
{
	// TODO: Uh, why aren't bows, crossbows, thrown missiles, no-ammo missiles included? (There's probably a reason though.)
	if((univ.party[pc_num].items[item].variety  == eItemType::ONE_HANDED) ||
		(univ.party[pc_num].items[item].variety  == eItemType::TWO_HANDED) ||
		(univ.party[pc_num].items[item].variety  == eItemType::ARROW) ||
		(univ.party[pc_num].items[item].variety  == eItemType::BOLTS))
		return true;
	else return false;
	
}

void cast_spell(eSkill type)
//short type; // 0 - mage  1 - priest
{
	eSpell spell;
	
	if((is_town()) && (univ.town.is_antimagic(univ.town.p_loc.x,univ.town.p_loc.y))) {
		add_string_to_buf("  Not in antimagic field.");
		return;
	}
	
	if(!spell_forced)
		spell = pick_spell(6, type);
	else {
		if(!repeat_cast_ok(type))
			return;
		spell = type == eSkill::MAGE_SPELLS ? store_mage : store_priest;
	}
	if(spell != eSpell::NONE) {
		print_spell_cast(spell,type);
		
		if(type == eSkill::MAGE_SPELLS)
			do_mage_spell(pc_casting,spell);
		else do_priest_spell(pc_casting,spell);
		put_pc_screen();
		
	}
}

bool repeat_cast_ok(eSkill type)
{
	short who_would_cast;
	eSpellSelect store_select;
	eSpell what_spell;
	
	if(overall_mode == MODE_COMBAT)
		who_would_cast = current_pc;
	else if(overall_mode < MODE_TALK_TOWN)
		who_would_cast = pc_casting;
	else return false;
	
	if(is_combat())
		what_spell = univ.party[who_would_cast].last_cast[type];
	else what_spell = type == eSkill::MAGE_SPELLS ? store_mage : store_priest;
	
	if(!pc_can_cast_spell(who_would_cast,what_spell)) {
		add_string_to_buf("Repeat cast: Can't cast.");
		return false;
	}
	store_select = (*what_spell).need_select;
	if(store_select != SELECT_NO && store_spell_target == 6) {
		add_string_to_buf("Repeat cast: No target stored.");
		return false;
	}
	if(store_select == SELECT_ANY &&
			isAbsent(univ.party[store_spell_target].main_status)) {
		add_string_to_buf("Repeat cast: No target stored.");
		return false;
	}
	if(store_select == SELECT_ACTIVE &&
	   univ.party[store_spell_target].main_status != eMainStatus::ALIVE) {
		add_string_to_buf("Repeat cast: No target stored.");
		return false;
	}
	
	return true;
	
}

void give_party_spell(short which) ////
//which; // 100 + x : priest spell x
{
	short i;
	bool sound_done = false;
	char str[60];
	
	if((which < 0) || (which > 161) || ((which > 61) && (which < 100))) {
		giveError("The scenario has tried to give you a non-existant spell.");
		return;}
	
	if(which < 100)
		for(i = 0; i < 6; i++)
			if(!univ.party[i].mage_spells[which]) {
				univ.party[i].mage_spells[which] = true;
				if(univ.party[i].main_status == eMainStatus::ALIVE)
					sprintf((char *) str,"%s learns spell.",univ.party[i].name.c_str());
				give_help(41,0);
				if(!sound_done) {
					sound_done = true;
					play_sound(62);
				};
			}
	if(which >= 100)
		for(i = 0; i < 6; i++)
			if(!univ.party[i].priest_spells[which - 100]) {
				univ.party[i].priest_spells[which - 100] = true;
				if(univ.party[i].main_status == eMainStatus::ALIVE)
					sprintf((char *) str,"%s learns spell.",univ.party[i].name.c_str());
				give_help(41,0);
				if(!sound_done) {
					sound_done = true;
					play_sound(62);
				}
			}
}

void do_mage_spell(short pc_num,eSpell spell_num)
{
	short i,j,item,target,r1,adj,store;
	location where;
	
	where = univ.town.p_loc;
	play_sound(25);
	current_spell_range = 8;
	store_mage = spell_num;
	
	adj = stat_adj(who_cast,eSkill::INTELLIGENCE);
	
	switch(spell_num) {
		case eSpell::LIGHT:
			univ.party[pc_num].cur_sp -= (*spell_num).cost;
			increase_light(50);
			break;
			
		case eSpell::IDENTIFY:
			univ.party[pc_num].cur_sp -= (*spell_num).cost;
			ASB("All of your items are identified.");
			for(i = 0; i < 6; i++)
				for(j = 0; j < 24; j++)
					univ.party[i].items[j].ident = true;
			break;
			
		case eSpell::TRUE_SIGHT:
			univ.party[pc_num].cur_sp -= (*spell_num).cost;
			for(where.x = 0; where.x < 64; where.x++)
				for(where.y = 0; where.y < 64; where.y++)
					if(dist(where,univ.town.p_loc) <= 2)
						make_explored(where.x,where.y);
			clear_map();
			break;
			
		case eSpell::SUMMON_BEAST:
			r1 = get_summon_monster(1);
			if(r1 < 0) break;
			univ.party[pc_num].cur_sp -= (*spell_num).cost;
			store = get_ran(3,1,4) + adj;
			if(!summon_monster(r1,where,store,2))
				add_string_to_buf("  Summon failed.");
			break;
		case eSpell::SUMMON_WEAK:
			store = univ.party[who_cast].level / 5 + stat_adj(who_cast,eSkill::INTELLIGENCE) / 3 + get_ran(1,0,2);
			j = minmax(1,7,store);
			r1 = get_summon_monster(1); ////
			if(r1 < 0) break;
			univ.party[pc_num].cur_sp -= (*spell_num).cost;
			store = get_ran(4,1,4) + adj;
			for(i = 0; i < j; i++)
				if(!summon_monster(r1,where,store,2))
					add_string_to_buf("  Summon failed.");
			break;
		case eSpell::SUMMON:
			store = univ.party[who_cast].level / 7 + stat_adj(who_cast,eSkill::INTELLIGENCE) / 3 + get_ran(1,0,1);
			j = minmax(1,6,store);
			r1 = get_summon_monster(2); ////
			if(r1 < 0) break;
			univ.party[pc_num].cur_sp -= (*spell_num).cost;
			store = get_ran(5,1,4) + adj;
			for(i = 0; i < j; i++)
				if(!summon_monster(r1,where,store,2))
					add_string_to_buf("  Summon failed.");
			break;
		case eSpell::SUMMON_MAJOR:
			store = univ.party[who_cast].level / 10 + stat_adj(who_cast,eSkill::INTELLIGENCE) / 3 + get_ran(1,0,1);
			j = minmax(1,5,store);
			r1 = get_summon_monster(3); ////
			if(r1 < 0) break;
			univ.party[pc_num].cur_sp -= (*spell_num).cost;
			store = get_ran(7,1,4) + stat_adj(who_cast,eSkill::INTELLIGENCE);
			for(i = 0; i < j; i++)
				if(!summon_monster(r1,where,store,2))
					add_string_to_buf("  Summon failed.");
			break;
		case eSpell::DEMON:
			store = get_ran(5,1,4) + 2 * stat_adj(who_cast,eSkill::INTELLIGENCE);
			if(!summon_monster(85,where,store,2))
				add_string_to_buf("  Summon failed.");
			else univ.party[pc_num].cur_sp -= (*spell_num).cost;
			break;
			
		case eSpell::DISPEL_SQUARE:
			add_string_to_buf("  Target spell.               ");
			current_pat = square;
			start_town_targeting(spell_num,pc_num);
			break;
			
		case eSpell::LIGHT_LONG:
			univ.party[pc_num].cur_sp -= (*spell_num).cost;
			increase_light(200);
			break;
			
		case eSpell::MAGIC_MAP:
			item = pc_has_abil(pc_num,eItemAbil::SAPPHIRE);
			if(item == 24)
				add_string_to_buf("  You need a sapphire.        ");
			else if(univ.town->defy_scrying || univ.town->defy_mapping)
				add_string_to_buf("  The spell fails.                ");
			else {
				remove_charge(pc_num,item);
				univ.party[pc_num].cur_sp -= (*spell_num).cost;
				add_string_to_buf("  As the sapphire dissolves,       ");
				add_string_to_buf("  you have a vision.               ");
				for(i = 0; i < 64; i++)
					for(j = 0; j < 64; j++)
						make_explored(i,j);
				clear_map();
			}
			break;
			
			
		case eSpell::STEALTH:
			univ.party[pc_num].cur_sp -= (*spell_num).cost;
			PSD[SDF_PARTY_STEALTHY] += max(6,univ.party[pc_num].level * 2);
			break;
			
			
		case eSpell::SCRY_MONSTER: case eSpell::UNLOCK: case eSpell::CAPTURE_SOUL: case eSpell::DISPEL_BARRIER:
		case eSpell::BARRIER_FIRE: case eSpell::BARRIER_FORCE: case eSpell::QUICKFIRE:
			add_string_to_buf("  Target spell.               ");
			current_pat = single;
			start_town_targeting(spell_num,pc_num);
			break;
			
		case eSpell::ANTIMAGIC:
			add_string_to_buf("  Target spell.               ");
			current_pat = rad2;
			start_town_targeting(spell_num,pc_num);
			break;
			
		case eSpell::FLIGHT:
			if(PSD[SDF_PARTY_FLIGHT] > 0) {
				add_string_to_buf("  Not while already flying.          ");
				return;
			}
			if(univ.party.in_boat >= 0)
				add_string_to_buf("  Leave boat first.             ");
			else if(univ.party.in_horse >= 0) ////
				add_string_to_buf("  Leave horse first.             ");
			else {
				univ.party[pc_num].cur_sp -= (*spell_num).cost;
				add_string_to_buf("  You start flying!               ");
				PSD[SDF_PARTY_FLIGHT] = 3;
			}
			break;
			
		case eSpell::RESIST_MAGIC: case eSpell::PROTECTION:
			target = store_spell_target;
			if(target < 6)
				univ.party[pc_num].cur_sp -= (*spell_num).cost;
			if(spell_num == eSpell::PROTECTION && target < 6) {
				univ.party[target].status[eStatus::INVULNERABLE] += 2 + stat_adj(pc_num,eSkill::INTELLIGENCE) + get_ran(2,1,2);
				for(i = 0; i < 6; i++)
					if(univ.party[i].main_status == eMainStatus::ALIVE) {
						univ.party[i].status[eStatus::MAGIC_RESISTANCE] += 4 + univ.party[pc_num].level / 3 + stat_adj(pc_num,eSkill::INTELLIGENCE);
					}
				sprintf ((char *) c_line, "  Party protected.                         ");
			}
			if(spell_num == eSpell::RESIST_MAGIC && target < 6) {
				univ.party[target].status[eStatus::MAGIC_RESISTANCE] += 2 + stat_adj(pc_num,eSkill::INTELLIGENCE) + get_ran(2,1,2);
				sprintf ((char *) c_line, "  %s protected.",univ.party[target].name.c_str());
			}
			add_string_to_buf((char *) c_line);
			break;
	}
}

void do_priest_spell(short pc_num,eSpell spell_num) ////
{
	short r1,r2, target, i,item,store,adj,x,y;
	location loc;
	location where;
	
	short store_victim_health,store_caster_health,targ_damaged; // for symbiosis
	
	where = univ.town.p_loc;
	
	adj = stat_adj(pc_num,eSkill::INTELLIGENCE);
	
	play_sound(24);
	current_spell_range = 8;
	store_priest = spell_num;
	
	switch(spell_num) {
		case eSpell::LOCATION:
			univ.party[pc_num].cur_sp -= (*spell_num).cost;
			
			if(is_town()) {
				loc = (overall_mode == MODE_OUTDOORS) ? univ.party.p_loc : univ.town.p_loc;
				sprintf ((char *) c_line, "  You're at: x %d  y %d.",
						 (short) loc.x, (short) loc.y);
			}
			if(is_out()) {
				loc = (overall_mode == MODE_OUTDOORS) ? univ.party.p_loc : univ.town.p_loc;
				x = loc.x; y = loc.y;
				x += 48 * univ.party.outdoor_corner.x; y += 48 * univ.party.outdoor_corner.y;
				sprintf ((char *) c_line, "  You're outside at: x %d  y %d.",x,y);
				
			}
			add_string_to_buf((char *) c_line);
			break;
			
		case eSpell::MANNA_MINOR: case eSpell::MANNA:
			univ.party[pc_num].cur_sp -= (*spell_num).cost;
			store = univ.party[pc_num].level / 3 + 2 * stat_adj(who_cast,eSkill::INTELLIGENCE) + get_ran(2,1,4);
			r1 = max(0,store);
			if(spell_num == eSpell::MANNA_MINOR)
				r1 = r1 / 3;
			sprintf ((char *) c_line, "  You gain %d food.   ",r1);
			add_string_to_buf((char *) c_line);
			give_food(r1,true);
			break;
			
		case eSpell::RITUAL_SANCTIFY:
			add_string_to_buf("  Sanctify which space?               ");
			current_pat = single;
			start_town_targeting(spell_num,pc_num);
			break;
			
		case eSpell::LIGHT_DIVINE:
			univ.party[pc_num].cur_sp -= (*spell_num).cost;
			// TODO: Should this call increase_light_level?
			univ.party.light_level += 210;
			break;
			
		case eSpell::SUMMON_SPIRIT:
			store = stat_adj(who_cast,eSkill::INTELLIGENCE);
			if(!summon_monster(125,where,get_ran(2,1,4) + store,2))
				add_string_to_buf("  Summon failed.");
			else univ.party[pc_num].cur_sp -= (*spell_num).cost;
			break;
		case eSpell::STICKS_TO_SNAKES:
			univ.party[pc_num].cur_sp -= (*spell_num).cost;
			r1 = univ.party[who_cast].level / 6 + stat_adj(who_cast,eSkill::INTELLIGENCE) / 3 + get_ran(1,0,1);
			for(i = 0; i < r1; i++) {
				r2 = get_ran(1,0,7);
				store = get_ran(2,1,5) + stat_adj(who_cast,eSkill::INTELLIGENCE);
				if(!summon_monster((r2 == 1) ? 100 : 99,where,store,2 ))
					add_string_to_buf("  Summon failed.");
			}
			break;
		case eSpell::SUMMON_HOST:
			univ.party[pc_num].cur_sp -= (*spell_num).cost;
			store = get_ran(2,1,4) + stat_adj(who_cast,eSkill::INTELLIGENCE);
			if(!summon_monster(126,where,store,2))
				add_string_to_buf("  Summon failed.");
			for(i = 0; i < 4; i++)	{
				store = get_ran(2,1,4) + stat_adj(who_cast,eSkill::INTELLIGENCE);
				if(!summon_monster(125,where,store,2))
					add_string_to_buf("  Summon failed.");
			}
			break;
		case eSpell::SUMMON_GUARDIAN:
			store = get_ran(6,1,4) + stat_adj(who_cast,eSkill::INTELLIGENCE);
			if(!summon_monster(122,where,store,2))
				add_string_to_buf("  Summon failed.");
			else univ.party[pc_num].cur_sp -= (*spell_num).cost;
			break;
			
		case eSpell::MOVE_MOUNTAINS: case eSpell::MOVE_MOUNTAINS_MASS:
			add_string_to_buf("  Destroy what?               ");
			current_pat = (spell_num == eSpell::MOVE_MOUNTAINS) ? single : square;
			start_town_targeting(spell_num,pc_num);
			break;
			
		case eSpell::DISPEL_SPHERE: case eSpell::DISPEL_FIELD:
			add_string_to_buf("  Target spell.               ");
			current_pat = (spell_num == eSpell::DISPEL_SPHERE) ? rad2 : single;
			start_town_targeting(spell_num,pc_num);
			break;
			
		case eSpell::DETECT_LIFE:
			add_string_to_buf("  Monsters now on map.                ");
			PSD[SDF_PARTY_DETECT_LIFE] += 6 + get_ran(1,0,6);
			clear_map();
			univ.party[pc_num].cur_sp -= (*spell_num).cost;
			break;
		case eSpell::FIREWALK:
			add_string_to_buf("  You are now firewalking.                ");
			PSD[SDF_PARTY_FIREWALK] += univ.party[pc_num].level / 12 + 2;
			univ.party[pc_num].cur_sp -= (*spell_num).cost;
			break;
			
		case eSpell::SHATTER:
			add_string_to_buf("  You send out a burst of energy. ");
			univ.party[pc_num].cur_sp -= (*spell_num).cost;
			for(loc.x = where.x - 1;loc.x < where.x + 2; loc.x++)
				for(loc.y = where.y - 1;loc.y < where.y + 2; loc.y++)
					crumble_wall(loc);
			update_explored(univ.town.p_loc);
			break;
			
		case eSpell::WORD_RECALL:
			if(overall_mode > MODE_OUTDOORS) {
				add_string_to_buf("  Can only cast outdoors. ");
				return;
			}
			if(univ.party.in_boat >= 0) {
				add_string_to_buf("  Not while in boat. ");
				return;
			}
			if(univ.party.in_horse >= 0) {////
				add_string_to_buf("  Not while on horseback. ");
				return;
			}
			univ.party[pc_num].cur_sp -= (*spell_num).cost;
			add_string_to_buf("  You are moved... ");
			force_town_enter(scenario.which_town_start,scenario.where_start);
			start_town_mode(scenario.which_town_start,9);
			position_party(scenario.out_sec_start.x,scenario.out_sec_start.y,
						   scenario.out_start.x,scenario.out_start.y);
			center = univ.town.p_loc = scenario.where_start;
//			overall_mode = MODE_OUTDOORS;
//			center = univ.party.p_loc;
//			update_explored(univ.party.p_loc);
			redraw_screen(REFRESH_ALL);
			break;
			
		case eSpell::HEAL_MINOR: case eSpell::HEAL: case eSpell::HEAL_MAJOR:
		case eSpell::POISON_WEAKEN: case eSpell::POISON_CURE: case eSpell::DISEASE_CURE:
		case eSpell::RESTORE_MIND: case eSpell::CLEANSE: case eSpell::AWAKEN: case eSpell::PARALYSIS_CURE:
//			target = select_pc(11,0);
			target = store_spell_target;
			if(target < 6) {
				univ.party[pc_num].cur_sp -= (*spell_num).cost;
				switch(spell_num) {
					case eSpell::HEAL_MINOR: case eSpell::HEAL: case eSpell::HEAL_MAJOR:
						if(spell_num == eSpell::HEAL_MINOR)
							r1 = get_ran(2, 1, 4);
						else r1 = get_ran(2 + (spell_num == eSpell::HEAL ? 6 : 12), 1, 4);
						sprintf ((char *) c_line, "  %s healed %d.   ",
								 (char *) univ.party[target].name.c_str(),r1);
						heal_pc(target,r1);
						one_sound(52);
						break;
						
					case eSpell::POISON_WEAKEN: case eSpell::POISON_CURE:
						sprintf ((char *) c_line, "  %s cured.    "
								 ,(char *) univ.party[target].name.c_str());
						r1 = ((spell_num == eSpell::POISON_WEAKEN) ? 1 : 3) + get_ran(1,0,2) + stat_adj(pc_num,eSkill::INTELLIGENCE) / 2;
						cure_pc(target,r1);
						break;
						
					case eSpell::AWAKEN:
						if(univ.party[target].status[eStatus::ASLEEP] <= 0) {
							sprintf ((char *) c_line, "  %s is already awake!    "
									 ,(char *) univ.party[target].name.c_str());
							break;
						}
						sprintf ((char *) c_line, "  %s wakes up.    "
								 ,(char *) univ.party[target].name.c_str());
						univ.party[target].status[eStatus::ASLEEP] = 0;
						break;
					case eSpell::PARALYSIS_CURE:
						if(univ.party[target].status[eStatus::PARALYZED] <= 0) {
							sprintf ((char *) c_line, "  %s isn't paralyzed!    "
									 ,(char *) univ.party[target].name.c_str());
							break;
						}
						sprintf ((char *) c_line, "  %s can move now.    "
								 ,(char *) univ.party[target].name.c_str());
						univ.party[target].status[eStatus::PARALYZED] = 0;
						break;
						
					case eSpell::DISEASE_CURE:
						sprintf ((char *) c_line, "  %s recovers.      "
								 ,(char *) univ.party[target].name.c_str());
						r1 = 2 + get_ran(1,0,2) + stat_adj(pc_num,eSkill::INTELLIGENCE) / 2;
						univ.party[target].status[eStatus::DISEASE] = max(0,univ.party[target].status[eStatus::DISEASE] - r1);
						break;
						
					case eSpell::RESTORE_MIND:
						sprintf ((char *) c_line, "  %s restored.      "
								 ,(char *) univ.party[target].name.c_str());
						r1 = 1 + get_ran(1,0,2) + stat_adj(pc_num,eSkill::INTELLIGENCE) / 2;
						univ.party[target].status[eStatus::DUMB] = max(0,univ.party[target].status[eStatus::DUMB] - r1);
						break;
						
					case eSpell::CLEANSE:
						sprintf ((char *) c_line, "  %s cleansed.      "
								 ,(char *) univ.party[target].name.c_str());
						univ.party[target].status[eStatus::DISEASE] = 0;
						univ.party[target].status[eStatus::WEBS] = 0;
						break;
				}
			}
			add_string_to_buf((char *) c_line);
			put_pc_screen();
			break;
			
		case eSpell::REVIVE: case eSpell::DESTONE: case eSpell::RAISE_DEAD: case eSpell::RESURRECT:
		case eSpell::CURSE_REMOVE: case eSpell::SANCTUARY: case eSpell::SYMBIOSIS: case eSpell::MARTYRS_SHIELD:
			target = store_spell_target;
			
			if(target < 6) {
				if(spell_num == eSpell::SYMBIOSIS && target == pc_num) {
					add_string_to_buf("  Can't cast on self.");
					return;
				}
				
				univ.party[pc_num].cur_sp -= (*spell_num).cost;
				if(spell_num == eSpell::MARTYRS_SHIELD) {
					sprintf ((char *) c_line, "  %s shielded.         ",
							 (char *) univ.party[target].name.c_str());
					r1 = max(1,get_ran((univ.party[pc_num].level + 5) / 5,1,3) + adj);
					univ.party[target].status[eStatus::MARTYRS_SHIELD] += r1;
				} else if(spell_num == eSpell::SANCTUARY) {
					sprintf ((char *) c_line, "  %s hidden.         ",
							 (char *) univ.party[target].name.c_str());
					r1 = max(0,get_ran(0,1,3) + univ.party[pc_num].level / 4 + adj);
					univ.party[target].status[eStatus::INVISIBLE] += r1;
				} else if(spell_num == eSpell::SYMBIOSIS) {
					store_victim_health = univ.party[target].cur_health;
					store_caster_health = univ.party[pc_num].cur_health;
					targ_damaged = univ.party[target].max_health - univ.party[target].cur_health;
					while((targ_damaged > 0) && (univ.party[pc_num].cur_health > 0)) {
						univ.party[target].cur_health++;
						r1 = get_ran(1,1,100) + univ.party[pc_num].level / 2 + 3 * adj;
						if(r1 < 100)
							univ.party[pc_num].cur_health--;
						if(r1 < 50)
							move_to_zero(univ.party[pc_num].cur_health);
						targ_damaged = univ.party[target].max_health - univ.party[target].cur_health;
					}
					add_string_to_buf("  You absorb damage.");
					sprintf ((char *) c_line, "  %s healed %d.         ", (char *) univ.party[target].name.c_str(),
							 univ.party[target].cur_health - store_victim_health);
					add_string_to_buf ((char *) c_line);
					sprintf ((char *) c_line, "  %s takes %d.         ", (char *) univ.party[pc_num].name.c_str(),
							 store_caster_health - univ.party[pc_num].cur_health);
				} else if(spell_num == eSpell::REVIVE) {
					sprintf ((char *) c_line, "  %s healed.         ",
							 (char *) univ.party[target].name.c_str());
					heal_pc(target,250);
					univ.party[target].status[eStatus::POISON] = 0;
					one_sound(-53); one_sound(52);
				} else if(spell_num == eSpell::DESTONE) {
					if(univ.party[target].main_status == eMainStatus::STONE) {
						univ.party[target].main_status = eMainStatus::ALIVE;
						sprintf ((char *) c_line, "  %s destoned.                                  ",
								 (char *) univ.party[target].name.c_str());
						play_sound(53);
					}
					else sprintf ((char *) c_line,"  Wasn't stoned.              ");
				} else if(spell_num == eSpell::CURSE_REMOVE) {
					for(i = 0; i < 24; i++)
						if(univ.party[target].items[i].cursed){
							r1 = get_ran(1,0,200) - 10 * stat_adj(pc_num,eSkill::INTELLIGENCE);
							if(r1 < 60) {
								univ.party[target].items[i].cursed = univ.party[target].items[i].unsellable = false;
							}
						}
					play_sound(52);
					sprintf ((char *) c_line,"  Your items glow.     ");
				} else {
					
					if(!PSD[SDF_RESURRECT_NO_BALM]) {
						if((item = pc_has_abil(pc_num,eItemAbil::RESSURECTION_BALM)) == 24) {
							add_string_to_buf("  Need resurrection balm.        ");
							break;
						}
						else take_item(pc_num,item);
					}
					if(spell_num == eSpell::RAISE_DEAD) {
						if(univ.party[target].main_status == eMainStatus::DEAD)
							if(get_ran(1,1,univ.party[pc_num].level / 2) == 1) {
								sprintf ((char *) c_line, "  %s now dust.                          ",
										 (char *) univ.party[target].name.c_str());
								play_sound(5);
								univ.party[target].main_status = eMainStatus::DUST;
							}
							else {
								univ.party[target].main_status = eMainStatus::ALIVE;
								for(i = 0; i < 3; i++)
									if(get_ran(1,0,2) < 2) {
										eSkill skill = eSkill(i);
										univ.party[target].skills[skill] -= (univ.party[target].skills[skill] > 1) ? 1 : 0;
									}
								univ.party[target].cur_health = 1;
								sprintf ((char *) c_line, "  %s raised.                          ",
										 (char *) univ.party[target].name.c_str());
								play_sound(52);
							}
							else sprintf ((char *) c_line,"  Didn't work.              ");
						
					} else if(spell_num == eSpell::RESURRECT) {
						if(univ.party[target].main_status != eMainStatus::ALIVE) {
							univ.party[target].main_status = eMainStatus::ALIVE;
							for(i = 0; i < 3; i++)
								if(get_ran(1,0,2) < 1) {
									eSkill skill = eSkill(i);
									univ.party[target].skills[skill] -= (univ.party[target].skills[skill] > 1) ? 1 : 0;
								}
							univ.party[target].cur_health = 1;
							sprintf ((char *) c_line, "  %s raised.                          ",
									 (char *) univ.party[target].name.c_str());
							play_sound(52);
						}
						else sprintf ((char *) c_line,"  Was OK.              ");
					}
				}
				add_string_to_buf((char *) c_line);
				put_pc_screen();
			}
			break;
			
		case eSpell::HEAL_ALL_LIGHT: case eSpell::HEAL_ALL: case eSpell::REVIVE_ALL:
			univ.party[pc_num].cur_sp -= (*spell_num).cost;
			if(spell_num != eSpell::REVIVE_ALL) {
				r1 = get_ran((spell_num == eSpell::HEAL_ALL ? 6 : 3) + adj, 1, 4);
				sprintf ((char *) c_line, "  Party healed %d.       ", r1);
				add_string_to_buf((char *) c_line);
				heal_party(r1);
				play_sound(52);
			} else {
				r1 = get_ran(7 + adj, 1, 4);
				sprintf ((char *) c_line, "  Party revived.     ");
				add_string_to_buf((char *) c_line);
				r1 = r1 * 2;
				heal_party(r1);
				play_sound(-53);
				play_sound(-52);
				cure_party(3 + adj);
			}
			break;
			
		case eSpell::POISON_CURE_ALL:
			univ.party[pc_num].cur_sp -= (*spell_num).cost;
			sprintf ((char *) c_line, "  Party cured.  ");
			add_string_to_buf((char *) c_line);
			cure_party(3 + stat_adj(pc_num,eSkill::INTELLIGENCE));
			break;
			
		case eSpell::SANCTUARY_MASS: case eSpell::CLEANSE_MAJOR: case eSpell::HYPERACTIVITY:
			univ.party[pc_num].cur_sp -= (*spell_num).cost;
			switch(spell_num) {
				case eSpell::SANCTUARY_MASS: add_string_to_buf("  Party hidden.");break;
				case eSpell::CLEANSE_MAJOR: add_string_to_buf("  Party cleansed.");break;
				case eSpell::HYPERACTIVITY: add_string_to_buf("  Party is now really, REALLY awake.");break;
			}
			
			for(i = 0; i < 6; i++)
				if(univ.party[i].main_status == eMainStatus::ALIVE) {
					if(spell_num == eSpell::SANCTUARY_MASS) {
						store = get_ran(0,1,3) + univ.party[pc_num].level / 6 + stat_adj(pc_num,eSkill::INTELLIGENCE);
						r1 = max(0,store);
						univ.party[i].status[eStatus::INVISIBLE] += r1;
					}
					if(spell_num == eSpell::CLEANSE_MAJOR) {
						univ.party[i].status[eStatus::WEBS] = 0;
						univ.party[i].status[eStatus::DISEASE] = 0;
					}
					if(spell_num == eSpell::HYPERACTIVITY) {
						// Looks like this isn't clipped to a positive number. (That's probably intentional.)
						// TODO: So, should a status icon be added for negative levels of sleep?
						univ.party[i].status[eStatus::ASLEEP] -= 6 + 2 * stat_adj(pc_num,eSkill::INTELLIGENCE);
						univ.party[i].status[eStatus::HASTE_SLOW] = max(0,univ.party[i].status[eStatus::HASTE_SLOW]);
					}
				}
			break;
	}
}

void cast_town_spell(location where) ////
{
	short adjust,r1,targ,store;
	location loc;
	ter_num_t ter;
	
	if((where.x <= univ.town->in_town_rect.left) ||
		(where.x >= univ.town->in_town_rect.right) ||
		(where.y <= univ.town->in_town_rect.top) ||
		(where.y >= univ.town->in_town_rect.bottom)) {
		add_string_to_buf("  Can't target outside town.");
		return;
	}
	
	adjust = can_see_light(univ.town.p_loc,where,sight_obscurity);
	eSkill spell_type = (*town_spell).type;
	if(!spell_freebie)
		univ.party[who_cast].cur_sp -= (*town_spell).cost;
	ter = univ.town->terrain(where.x,where.y);
	
	// TODO: Should we do this here? Or in the handling of targeting modes?
	// (It really depends whether we want to be able to trigger it for targeting something other than a spell.)
	if(adjust <= 4 && !cast_spell_on_space(where, town_spell))
		return; // The special node intercepted and cancelled regular spell behaviour.
	
	if(adjust > 4)
		add_string_to_buf("  Can't see target.       ");
	else switch(town_spell) {
		case eSpell::SCRY_MONSTER: case eSpell::CAPTURE_SOUL:
			targ = monst_there(where);
			if(targ < univ.town->max_monst()) {
				if(town_spell == eSpell::SCRY_MONSTER) {
					univ.party.m_noted[univ.town.monst[targ].number] = true;
					adjust_monst_menu();
					display_monst(0,&univ.town.monst[targ],0);
				}
				else record_monst(&univ.town.monst[targ]);
			}
			else add_string_to_buf("  No monster there.");
			break;
			
		case eSpell::DISPEL_FIELD: case eSpell::DISPEL_SPHERE: case eSpell::DISPEL_SQUARE:
			add_string_to_buf("  You attempt to dispel.              ");
			place_spell_pattern(current_pat,where,FIELD_DISPEL,7);
			break;
		case eSpell::MOVE_MOUNTAINS:
		case eSpell::MOVE_MOUNTAINS_MASS:
			add_string_to_buf("  You blast the area.              ");
			place_spell_pattern(current_pat, where, FIELD_SMASH, 7);
			update_explored(univ.town.p_loc);
			break;
		case eSpell::BARRIER_FIRE:
			if(sight_obscurity(where.x,where.y) == 5 || monst_there(where) < 90) {
				add_string_to_buf("  Target space obstructed.");
				break;
			}
			univ.town.set_fire_barr(where.x,where.y,true);
			if(univ.town.is_fire_barr(where.x,where.y))
				add_string_to_buf("  You create the barrier.              ");
			else add_string_to_buf("  Failed.");
			break;
		case eSpell::BARRIER_FORCE:
			if(sight_obscurity(where.x,where.y) == 5 || monst_there(where) < 90) {
				add_string_to_buf("  Target space obstructed.");
				break;
			}
			univ.town.set_force_barr(where.x,where.y,true);
			if(univ.town.is_force_barr(where.x,where.y))
				add_string_to_buf("  You create the barrier.              ");
			else add_string_to_buf("  Failed.");
			break;
		case eSpell::QUICKFIRE:
			univ.town.set_quickfire(where.x,where.y,true);
			if(univ.town.is_quickfire(where.x,where.y))
				add_string_to_buf("  You create quickfire.              ");
			else add_string_to_buf("  Failed.");
			break;
			
		case eSpell::ANTIMAGIC:
			add_string_to_buf("  You create an antimagic cloud.              ");
			for(loc.x = 0; loc.x < univ.town->max_dim(); loc.x++)
				for(loc.y = 0; loc.y < univ.town->max_dim(); loc.y++)
					if(dist(where,loc) <= 2 && can_see(where,loc,sight_obscurity) < 5 &&
					   ((abs(loc.x - where.x) < 2) || (abs(loc.y - where.y) < 2)))
						univ.town.set_antimagic(loc.x,loc.y,true);
			break;
			
		case eSpell::RITUAL_SANCTIFY:
			// Nothing to do here anymore; the code that used to be here is now just above the switch statement.
			break;
			
		case eSpell::UNLOCK:
			// TODO: Is the unlock spell supposed to have a max range?
			if(scenario.ter_types[ter].special == eTerSpec::UNLOCKABLE){
				if(scenario.ter_types[ter].flag2.u == 10)
					r1 = 10000;
				else{
					r1 = get_ran(1,1,100) - 5 * stat_adj(who_cast,eSkill::INTELLIGENCE) + 5 * univ.town.difficulty;
					r1 += scenario.ter_types[ter].flag2.u * 7;
				}
				if(r1 < (135 - combat_percent[min(19,univ.party[who_cast].level)])) {
					add_string_to_buf("  Door unlocked.                 ");
					play_sound(9);
					univ.town->terrain(where.x,where.y) = scenario.ter_types[ter].flag1.u;
				}
				else {
					play_sound(41);
					add_string_to_buf("  Didn't work.                  ");
				}
			}else
				add_string_to_buf("  Wrong terrain type.               ");
			break;
			
		case eSpell::DISPEL_BARRIER:
			if((univ.town.is_fire_barr(where.x,where.y)) || (univ.town.is_force_barr(where.x,where.y))) {
				r1 = get_ran(1,1,100) - 5 * stat_adj(who_cast,eSkill::INTELLIGENCE) + 5 * (univ.town.difficulty / 10);
				if(univ.town.is_fire_barr(where.x,where.y))
					r1 -= 8;
				if(r1 < (120 - combat_percent[min(19,univ.party[who_cast].level)])) {
					add_string_to_buf("  Barrier broken.                 ");
					univ.town.set_fire_barr(where.x,where.y,false);
					univ.town.set_force_barr(where.x,where.y,false);
					
					// Now, show party new things
					update_explored(univ.town.p_loc);
				}
				else {
					store = get_ran(1,0,1);
					play_sound(41);
					add_string_to_buf("  Didn't work.                  ");
				}
			} else if(univ.town.is_force_cage(where.x,where.y)) {
				add_string_to_buf("  Cage broken.");
				univ.town.set_force_cage(where.x,where.y,false);
			}
			
			else add_string_to_buf("  No barrier there.");
			
			break;
			
	}
}

bool cast_spell_on_space(location where, eSpell spell)
{
	short i,s1 = 0,s2 = 0,s3 = 0;
	
	for(i = 0; i < 50; i++)
		if(where == univ.town->special_locs[i]) {
			if(univ.town->specials[univ.town->spec_id[i]].type == eSpecType::IF_CONTEXT)
				run_special(eSpecCtx::TARGET,2,univ.town->spec_id[i],where,&s1,&s2,&s3);
			if(s3) redraw_terrain();
			return !s1;
		}
	if(spell == eSpell::RITUAL_SANCTIFY)
		add_string_to_buf("  Nothing happens.");
	return true;
}

void crumble_wall(location where) // TODO: Add something like this to the spreading quickfire function
{
	ter_num_t ter;
	
	if(loc_off_act_area(where))
		return;
	ter = univ.town->terrain(where.x,where.y);
	if(scenario.ter_types[ter].special == eTerSpec::CRUMBLING && scenario.ter_types[ter].flag3.u < 2) {
		play_sound(60);
		univ.town->terrain(where.x,where.y) = scenario.ter_types[ter].flag1.u;
		add_string_to_buf("  Barrier crumbles.");
	}
	
}

void do_mindduel(short pc_num,cCreature *monst)
{
	short i = 0,adjust,r1,r2,balance = 0;
	
	adjust = (univ.party[pc_num].level + univ.party[pc_num].skills[eSkill::INTELLIGENCE]) / 2 - monst->level * 2;
	if((i = get_prot_level(pc_num,eItemAbil::WILL)) > 0)
		adjust += i * 5;
	if(monst->attitude % 2 != 1)
		make_town_hostile();
	monst->attitude = 1;
	
	add_string_to_buf("Mindduel!");
	while(univ.party[pc_num].main_status == eMainStatus::ALIVE && monst->active > 0 && i < 10) {
		play_sound(1);
		r1 = get_ran(1,1,100) + adjust;
		r1 += 5 * (monst->status[eStatus::DUMB] - univ.party[pc_num].status[eStatus::DUMB]);
		r1 += 5 * balance;
		r2 = get_ran(1,1,6);
		if(r1 < 30) {
			sprintf((char *)c_line, "  %s is drained %d.",univ.party[pc_num].name.c_str(),r2);
			add_string_to_buf((char *) c_line);
			monst->mp += r2;
			balance++;
			if(univ.party[pc_num].cur_sp == 0) {
				univ.party[pc_num].status[eStatus::DUMB] += 2;
				sprintf((char *) c_line,"  %s is dumbfounded.",univ.party[pc_num].name.c_str());
				add_string_to_buf((char *) c_line);
				if(univ.party[pc_num].status[eStatus::DUMB] > 7) {
					sprintf((char *) c_line,"  %s is killed!",univ.party[pc_num].name.c_str());
					add_string_to_buf((char *) c_line);
					kill_pc(pc_num,eMainStatus::DEAD);
				}
				
			}
			else {
				univ.party[pc_num].cur_sp = max(0,univ.party[pc_num].cur_sp - r2);
			}
		}
		if(r1 > 70) {
			sprintf((char *)c_line, "  %s drains %d.",univ.party[pc_num].name.c_str(),r2);
			add_string_to_buf((char *) c_line);
			univ.party[pc_num].cur_sp += r2;
			balance--;
			if(monst->mp == 0) {
				monst->status[eStatus::DUMB] += 2;
				monst_spell_note(monst->number,22);
				if(monst->status[eStatus::DUMB] > 7) {
					kill_monst(monst,pc_num);
				}
				
			}
			else {
				monst->mp = max(0,monst->mp - r2);
			}
			
			
		}
		print_buf();
		i++;
	}
}

// mode 0 - dispel spell, 1 - always take  2 - always take and take fire and force too
void dispel_fields(short i,short j,short mode)
{
	short r1;
	
	if(mode == 2) {
		univ.town.set_fire_barr(i,j,false);
		univ.town.set_force_barr(i,j,false);
		univ.town.set_barrel(i,j,false);
		univ.town.set_crate(i,j,false);
		univ.town.set_web(i,j,false);
	}
	if(mode >= 1)
		mode = -10;
	univ.town.set_fire_wall(i,j,false);
	univ.town.set_force_wall(i,j,false);
	univ.town.set_scloud(i,j,false);
	r1 = get_ran(1,1,6) + mode;
	if(r1 <= 1)
		univ.town.set_web(i,j,false);
	r1 = get_ran(1,1,6) + mode;
	if(r1 < 6)
		univ.town.set_ice_wall(i,j,false);
	r1 = get_ran(1,1,6) + mode;
	if(r1 < 5)
		univ.town.set_sleep_cloud(i,j,false);
	r1 = get_ran(1,1,8) + mode;
	if(r1 <= 1)
		univ.town.set_quickfire(i,j,false);
	r1 = get_ran(1,1,7) + mode;
	if(r1 < 5)
		univ.town.set_blade_wall(i,j,false);
	r1 = get_ran(1,1,12) + mode;
	if(r1 < 3)
		univ.town.set_force_cage(i,j,false);
}

bool pc_can_cast_spell(short pc_num,eSkill type) {
	short store_w_cast;
	
	if(type == eSkill::MAGE_SPELLS && !pc_can_cast_spell(pc_num, eSpell::LIGHT))
		return false;
	if(type == eSkill::PRIEST_SPELLS && !pc_can_cast_spell(pc_num, eSpell::BLESS_MINOR))
		return false;
	
	// If they can't cast the most basic level 1 spell, let's just make sure they can't cast any spells.
	// Find a spell they definitely know, and see if they can cast that.
	if(type == eSkill::MAGE_SPELLS) {
		for(int i = 0; i < 62; i++)
			if(univ.party[pc_num].mage_spells[i])
				return pc_can_cast_spell(pc_num, eSpell(i));
	}
	if(type == eSkill::PRIEST_SPELLS) {
		for(int i = 0; i < 62; i++)
			if(univ.party[pc_num].priest_spells[i])
				return pc_can_cast_spell(pc_num, eSpell(i + 100));
	}
	// If we get this far, either they don't know any spells (very unlikely) or they can't cast any of the spells they know.
	return false;
}

bool pc_can_cast_spell(short pc_num,eSpell spell_num)
{
	short level,store_w_cast;
	eSkill type = (*spell_num).type;
	
	level = (*spell_num).level;
	
	if(overall_mode >= MODE_TALKING)
		return false; // From Windows version. It does kinda make sense, though this function shouldn't even be called in these modes.
	if(!isMage(spell_num) && !isPriest(spell_num))
		return false;
	if(univ.party[pc_num].skills[type] < level)
		return false;
	if(univ.party[pc_num].main_status != eMainStatus::ALIVE)
		return false;
	if(univ.party[pc_num].cur_sp < (*spell_num).cost)
		return false;
	// TODO: Maybe get rid of the casts here?
	if(type == eSkill::MAGE_SPELLS && !univ.party[pc_num].mage_spells[int(spell_num)])
		return false;
	if(type == eSkill::PRIEST_SPELLS && !univ.party[pc_num].priest_spells[int(spell_num) - 100])
		return false;
	if(univ.party[pc_num].status[eStatus::DUMB] >= 8 - level)
		return false;
	if(univ.party[pc_num].status[eStatus::PARALYZED] != 0)
		return false;
	if(univ.party[pc_num].status[eStatus::ASLEEP] > 0)
		return false;
	
	store_w_cast = (*spell_num).when_cast;
	if(is_out() && WHEN_OUTDOORS &~ store_w_cast)
		return false;
	if(is_town() && WHEN_TOWN &~ store_w_cast)
		return false;
	if(is_combat() &&  WHEN_COMBAT &~ store_w_cast)
		return false;
	return true;
}

// MARK: Begin spellcasting dialog

static void draw_caster_buttons(cDialog& me, const eSkill store_situation) {
	short i;
	
	for(i = 0; i < 6; i++) {
		std::string id = "caster" + boost::lexical_cast<std::string>(i + 1);
		if(!can_choose_caster) {
			if(i == pc_casting) {
				me[id].show();
			}
			else {
				me[id].hide();
			}
		}
		else {
			if(pc_can_cast_spell(i,store_situation)) {
				me[id].show();
			}
			else {
				me[id].hide();
			}
		}
	}
}

static void draw_spell_info(cDialog& me, const eSkill store_situation, const short store_spell) {
	
	
	if(store_spell == 70) { // No spell selected
		for(int i = 0; i < 6; i++) {
			std::string id = "target" + boost::lexical_cast<std::string>(i + 1);
			me[id].hide();
		}
		
	}
	else { // Spell selected
		
		for(int i = 0; i < 6; i++) {
			std::string id = "target" + boost::lexical_cast<std::string>(i + 1);
			// TODO: Make this thing an enum
			switch((*cSpell::fromNum(store_situation,store_spell)).need_select) {
				case SELECT_NO:
					me[id].hide();
					break;
				case SELECT_ACTIVE:
					if(univ.party[i].main_status != eMainStatus::ALIVE) {
						me[id].hide();
					}
					else {
						me[id].show();
					}
					break;
				case SELECT_ANY:
					// TODO: Split off party members should probably be excluded too?
					if(univ.party[i].main_status != eMainStatus::ABSENT) {
						me[id].show();
					}
					else {
						me[id].hide();
					}
					break;
					
			}
		}
	}
}

// TODO: Use the map here to simplify drawing the graphics in draw_pc_effects()
static void put_target_status_graphics(cDialog& me, short for_pc) {
	// First icon is displayed for positive values, second for negative, if -1 no negative icon.
	// This omits two special cases - major poison, and normal speed; they are hard-coded.
	static std::map<eStatus, std::pair<short, short>> statIcons = {
		{eStatus::POISONED_WEAPON, {4,-1}}, {eStatus::BLESS_CURSE, {2,3}}, {eStatus::POISON, {0,-1}},
		{eStatus::HASTE_SLOW, {6,8}}, {eStatus::INVULNERABLE, {5,-1}}, {eStatus::MAGIC_RESISTANCE, {9,19}},
		{eStatus::WEBS, {10,-1}}, {eStatus::DISEASE, {11,-1}}, {eStatus::INVISIBLE, {12,-1}},
		{eStatus::DUMB, {13,18}}, {eStatus::MARTYRS_SHIELD, {14,-1}}, {eStatus::ASLEEP, {15,-1}},
		{eStatus::PARALYZED, {16,-1}}, {eStatus::ACID, {17,-1}}, {eStatus::FORCECAGE, {20,-1}},
	};
	bool isAlive = univ.party[for_pc].main_status == eMainStatus::ALIVE;
	univ.party[for_pc].status[eStatus::HASTE_SLOW]; // This just makes sure it exists in the map, without changing its value if it does
	std::string id = "pc" + std::to_string(for_pc + 1);
	int slot = 0;
	for(auto next : univ.party[for_pc].status) {
		std::string id2 = id + "-stat" + std::to_string(slot + 1);
		cPict& pic = dynamic_cast<cPict&>(me[id2]);
		pic.setFormat(TXT_FRAME, false);
		if(isAlive) {
			short placedIcon = -1;
			if(next.first == eStatus::POISON && next.second > 4) placedIcon = 1;
			else if(next.second > 0) placedIcon = statIcons[next.first].first;
			else if(next.second < 0) placedIcon = statIcons[next.first].second;
			else if(next.first == eStatus::HASTE_SLOW) placedIcon = 7;
			if(placedIcon >= 0) {
				pic.show();
				pic.setPict(placedIcon);
				slot++;
			} else pic.hide();
		} else pic.hide();
	}
	while(slot < 15) {
		std::string id2 = id + "-stat" + std::to_string(slot + 1);
		me[id2].hide();
		slot++;
	}
}

static void draw_spell_pc_info(cDialog& me)
{
	short i;
	
	for(i = 0; i < 6; i++) {
		std::string n = boost::lexical_cast<std::string>(i + 1);
		if(univ.party[i].main_status != eMainStatus::ABSENT) {
			me["pc" + n].setText(univ.party[i].name);
			//if(pc_casting == i)
			//	cd_text_frame(1098,24 + store_spell_target,11);
			//else cd_text_frame(1098,24 + store_spell_target,1);
			
			if(univ.party[i].main_status == eMainStatus::ALIVE) {
				me["hp" + n].setTextToNum(univ.party[i].cur_health);
				me["sp" + n].setTextToNum(univ.party[i].cur_sp);
			}
			put_target_status_graphics(me, i);
		}
	}
}


static void put_pc_caster_buttons(cDialog& me)
{
	
	short i;
	
	for(i = 0; i < 6; i++) {
		std::string n = boost::lexical_cast<std::string>(i + 1);
		if(me["caster" + n].isVisible()) {
			if(i == pc_casting)
				me["pc" + n].setColour(sf::Color::Red);
			else me["pc" + n].setColour(me.getDefTextClr());
		}
	}
}

static void put_pc_target_buttons(cDialog& me, short& store_last_target_darkened) {
	
	if(store_spell_target < 6) {
		std::string n = boost::lexical_cast<std::string>(store_spell_target + 1);
		me["hp" + n].setColour(sf::Color::Red);
		me["sp" + n].setColour(sf::Color::Red);
	}
	if((store_last_target_darkened < 6) && (store_last_target_darkened != store_spell_target)) {
		std::string n = boost::lexical_cast<std::string>(store_last_target_darkened + 1);
		me["hp" + n].setColour(me.getDefTextClr());
		me["sp" + n].setColour(me.getDefTextClr());
	}
	store_last_target_darkened = store_spell_target;
}

// TODO: This stuff may be better handled by using an LED group with a custom focus handler
static void put_spell_led_buttons(cDialog& me, const eSkill store_situation,const short store_spell) {
	short i,spell_for_this_button;
	
	for(i = 0; i < 38; i++) {
		spell_for_this_button = (on_which_spell_page == 0) ? i : spell_index[i];
		std::string id = "spell" + boost::lexical_cast<std::string>(i + 1);
		cLed& led = dynamic_cast<cLed&>(me[id]);
		
		if(spell_for_this_button < 90) {
			eSpell spell = cSpell::fromNum(store_situation, spell_for_this_button);
			if(store_spell == spell_for_this_button) {
				led.setState(led_green);
			}
			else if(pc_can_cast_spell(pc_casting,spell)) {
				led.setState(led_red);
			}
			else {
				led.setState(led_off);
			}
		}
	}
}

static void put_spell_list(cDialog& me, const eSkill store_situation) {
	
	short i;
	char add_text[256];
	
	if(on_which_spell_page == 0) {
		me["col1"].setText("Level 1:");
		me["col2"].setText("Level 2:");
		me["col3"].setText("Level 3:");
		me["col4"].setText("Level 4:");
		for(i = 0; i < 38; i++) {
			std::string id = "spell" + boost::lexical_cast<std::string>(i + 1);
			auto& names = (store_situation == eSkill::MAGE_SPELLS ? mage_s_name : priest_s_name);
			if((*cSpell::fromNum(store_situation,i)).cost < 0) { // Simulacrum, which has a variable cost
				sprintf((char *) add_text,"%s (?)", names[i]);
			} else sprintf((char *) add_text,"%s (%d)", names[i], (*cSpell::fromNum(store_situation,i)).cost);
			//for(j = 0; j < 30; i++)
			//	if(add_text[j] == '&')
			//		add_text[j] = (char) ((97 + i > 122) ? 65 + (i - 26) : 97 + i);
			me[id].setText(add_text);
			if(spell_index[i] == 90)
				me[id].show();
		}
	}
	else {
		me["col1"].setText("Level 5:");
		me["col2"].setText("Level 6:");
		me["col3"].setText("Level 7:");
		me["col4"].setText("");
		for(i = 0; i < 38; i++) {
			std::string id = "spell" + boost::lexical_cast<std::string>(i + 1);
			auto& names = (store_situation == eSkill::MAGE_SPELLS ? mage_s_name : priest_s_name);
			if(spell_index[i] < 90) {
				sprintf(add_text, "%s (%d)", names[spell_index[i]], (*cSpell::fromNum(store_situation,i)).cost);
				me[id].setText(add_text);
			}
			else me[id].hide();
		}
	}
}

static bool pick_spell_caster(cDialog& me, std::string id, const eSkill store_situation, short& last_darkened, short& store_spell) {
	short item_hit = id[id.length() - 1] - '1';
	// TODO: This visibility check is probably not needed; wouldn't the dialog framework only trigger on visible elements?
	if(me[id].isVisible()) {
		pc_casting = item_hit;
		if(!pc_can_cast_spell(pc_casting,cSpell::fromNum(store_situation,store_spell))) {
			if(store_situation == eSkill::MAGE_SPELLS)
				store_spell = 70;
			else store_spell = 70;
			store_spell_target = 6;
		}
		draw_spell_info(me, store_situation,store_spell);
		draw_spell_pc_info(me);
		put_spell_led_buttons(me, store_situation,store_spell);
		put_pc_caster_buttons(me);
		put_pc_target_buttons(me, last_darkened);
	}
	return true;
}

static bool pick_spell_target(cDialog& me, std::string id, const eSkill store_situation, short& last_darkened, const short store_spell) {
	static const char*const no_target = " No target needed.";
	static const char*const bad_target = " Can't cast on him/her.";
	static const char*const got_target = " Target selected.";
	short item_hit = id[id.length() - 1] - '1';
	std::string casting = id;
	casting[casting.length() - 1] = pc_casting + '1';
	if(!me[casting].isVisible()) {
		me["feedback"].setText(no_target);
	} else if(!me[id].isVisible()) {
		me["feedback"].setText(bad_target);
	} else {
		me["feedback"].setText(got_target);
		store_spell_target = item_hit;
		draw_spell_info(me, store_situation, store_spell);
		put_pc_target_buttons(me, last_darkened);
	}
	return true;
}

static bool pick_spell_event_filter(cDialog& me, std::string item_hit, const eSkill store_situation, const short store_spell) {
	if(item_hit == "other") {
		on_which_spell_page = 1 - on_which_spell_page;
		put_spell_list(me, store_situation);
		put_spell_led_buttons(me, store_situation, store_spell);
	} else if(item_hit == "help") {
		univ.party.help_received[7] = 0;
		give_help(207,8,me);
	}
	return true;
}

static bool pick_spell_select_led(cDialog& me, std::string id, eKeyMod mods, const eSkill store_situation, short& last_darkened, short& store_spell) {
	static const char*const choose_target = " Now pick a target.";
	static const char*const bad_spell = " Spell not available.";
	short item_hit = boost::lexical_cast<short>(id.substr(5)) - 1;
	if(mod_contains(mods, mod_alt)) {
		int i = (on_which_spell_page == 0) ? item_hit : spell_index[item_hit];
		display_spells(store_situation,i,&me);
	}
	else if(dynamic_cast<cLed&>(me[id]).getState() == led_off) {
		me["feedback"].setText(bad_spell);
	}
	else {
		if(store_situation == eSkill::MAGE_SPELLS)
			store_spell = (on_which_spell_page == 0) ? item_hit : spell_index[item_hit];
		else store_spell = (on_which_spell_page == 0) ? item_hit : spell_index[item_hit];
		draw_spell_info(me, store_situation, store_spell);
		put_spell_led_buttons(me, store_situation, store_spell);
		
		if(store_spell_target < 6) {
			std::string targ = "target" + boost::lexical_cast<std::string>(store_spell_target + 1);
			if(!me[targ].isVisible()) {
				store_spell_target = 6;
				draw_spell_info(me, store_situation, store_spell);
				put_pc_target_buttons(me, last_darkened);
			}
		}
		// Cute trick now... if a target is needed, caster can always be picked
		std::string targ = "target" + boost::lexical_cast<std::string>(pc_casting + 1);
		if((store_spell_target == 6) && me[targ].isVisible()) {
			me["feedback"].setText(choose_target);
			draw_spell_info(me, store_situation, store_spell);
			play_sound(45); // formerly force_play_sound
		}
		else if(!me[targ].isVisible()) {
			store_spell_target = 6;
			put_pc_target_buttons(me, last_darkened);
		}
		
	}
	return true;
}

static bool finish_pick_spell(cDialog& me, bool spell_toast, const short store_store_target, const short& store_spell, const eSkill store_situation) {
	if(spell_toast) {
		store_spell_target = store_store_target ;
		if(store_situation == eSkill::MAGE_SPELLS)
			store_last_cast_mage = pc_casting;
		else store_last_cast_priest = pc_casting;
		me.toast(false);
		me.setResult<short>(70);
		return true;
	}
	
	eSpell picked_spell = cSpell::fromNum(store_situation,store_spell);
	if(store_spell == 70) {
		add_string_to_buf("Cast: No spell selected.");
		store_spell_target = store_store_target ;
		me.toast(false);
		me.setResult<short>(70);
		return true;
	}
	if(store_situation == eSkill::MAGE_SPELLS && (*picked_spell).need_select == SELECT_NO) {
		store_last_cast_mage = pc_casting;
		univ.party[pc_casting].last_cast[store_situation] = picked_spell;
		me.toast(false);
		me.setResult<short>(store_spell);
		return true;
	}
	if(store_situation == eSkill::PRIEST_SPELLS && (*picked_spell).need_select == SELECT_NO) {
		store_last_cast_priest = pc_casting;
		univ.party[pc_casting].last_cast[store_situation] = picked_spell;
		me.toast(false);
		me.setResult<short>(store_spell);
		return true;
	}
	if(store_spell_target == 6) {
		add_string_to_buf("Cast: Need to select target.");
		store_spell_target = store_store_target ;
		me.toast(false);
		give_help(39,0,me);
		me.setResult<short>(70);
		return true;
	}
	me.setResult<short>((store_situation == eSkill::MAGE_SPELLS) ? store_spell : store_spell);
	if(store_situation == eSkill::MAGE_SPELLS)
		store_last_cast_mage = pc_casting;
	else store_last_cast_priest = pc_casting;
	univ.party[pc_casting].last_cast[store_situation] = picked_spell;
	me.toast(true);
	return true;
}

eSpell pick_spell(short pc_num,eSkill type)  // 70 - no spell OW spell num
//short pc_num; // if 6, anyone
//short type; // 0 - mage   1 - priest
//short situation; // 0 - out  1 - town  2 - combat
{
	using namespace std::placeholders;
	eSpell store_spell = type == eSkill::MAGE_SPELLS ? store_mage : store_priest;
	short former_target = store_spell_target;
//	store_situation = type;
	short dark = 6;
	can_choose_caster = (pc_num < 6) ? false : true;
	
	pc_casting = type == eSkill::MAGE_SPELLS ? store_last_cast_mage : store_last_cast_priest;
	if(pc_casting == 6)
		pc_casting = current_pc;
	
	if(pc_num == 6) { // See if can keep same caster
		can_choose_caster = true;
		if(!pc_can_cast_spell(pc_casting,type)) {
			int i;
			for(i = 0; i < 6; i++)
				if(pc_can_cast_spell(i,type)) {
					pc_casting = i;
					i = 500;
				}
			if(i == 6) {
				add_string_to_buf("Cast: Nobody can.");
				return eSpell::NONE;
			}
		}
	}
	else {
		can_choose_caster = false;
		pc_casting = pc_num;
	}
	
	if(!can_choose_caster) {
		if(univ.party[pc_num].skills[type] == 0) {
			if(type == eSkill::MAGE_SPELLS) add_string_to_buf("Cast: No mage skill.");
			else add_string_to_buf("Cast: No priest skill.");
			return eSpell::NONE;
		}
		if(univ.party[pc_casting].cur_sp == 0) {
			add_string_to_buf("Cast: No spell points.");
			return eSpell::NONE;
		}
		
	}
	
	// If in combat, make the spell being cast this PCs most recent spell
	if(is_combat()) {
		if(type == eSkill::MAGE_SPELLS)
			store_spell = univ.party[pc_casting].last_cast[eSkill::MAGE_SPELLS];
		else store_spell = univ.party[pc_casting].last_cast[eSkill::PRIEST_SPELLS];
	}
	
	// Keep the stored spell, if it's still castable
	if(!pc_can_cast_spell(pc_casting,store_spell)) {
		if(type == eSkill::MAGE_SPELLS) {
			store_spell = eSpell::LIGHT;
		}
		else {
			store_spell = eSpell::HEAL_MINOR;
		}
	}
	
	// If a target is needed, keep the same target if that PC still targetable
	if(store_spell_target < 6) {
		if((*store_spell).need_select != SELECT_NO) {
			if(univ.party[store_spell_target].main_status != eMainStatus::ALIVE)
				store_spell_target = 6;
		} else store_spell_target = 6;
	}
	
	short former_spell = int(store_spell) % 100;
	// Set the spell page, based on starting spell
	if(former_spell >= 38) on_which_spell_page = 1;
	else on_which_spell_page = 0;
	
	
	make_cursor_sword();
	
	cDialog castSpell("cast-spell.xml");
	
	castSpell.attachClickHandlers(std::bind(pick_spell_caster, _1, _2, type, std::ref(dark), std::ref(former_spell)), {"caster1","caster2","caster3","caster4","caster5","caster6"});
	castSpell.attachClickHandlers(std::bind(pick_spell_target,_1,_2, type, dark, former_spell), {"target1","target2","target3","target4","target5","target6"});
	castSpell.attachClickHandlers(std::bind(pick_spell_event_filter, _1, _2, type, former_spell), {"other", "help"});
	castSpell["cast"].attachClickHandler(std::bind(finish_pick_spell, _1, false, former_target, std::ref(former_spell), type));
	castSpell["cancel"].attachClickHandler(std::bind(finish_pick_spell, _1, true, former_target, std::ref(former_spell), type));
	
	dynamic_cast<cPict&>(castSpell["pic"]).setPict(14 + (type == eSkill::PRIEST_SPELLS),PIC_DLOG);
	for(int i = 0; i < 38; i++) {
		std::string id = "spell" + boost::lexical_cast<std::string>(i + 1);
		cKey key;
		if(i > 25)
			key = {false, static_cast<unsigned char>('a' + i - 26), mod_shift};
		else key = {false, static_cast<unsigned char>('a' + i), mod_none};
		cLed& led = dynamic_cast<cLed&>(castSpell[id]);
		led.attachKey(key);
		castSpell.addLabelFor(id, {i > 25 ? toupper(key.c) : key.c}, LABEL_LEFT, 8, true);
		led.setState((pc_can_cast_spell(pc_casting,cSpell::fromNum(type,on_which_spell_page == 0 ? i : spell_index[i])))
					 ? led_red : led_green);
		led.attachClickHandler(std::bind(pick_spell_select_led, _1, _2, _3, type, std::ref(dark), std::ref(former_spell)));
	}
	
	put_spell_list(castSpell, type);
	draw_spell_info(castSpell, type, former_spell);
	put_pc_caster_buttons(castSpell);
	draw_spell_pc_info(castSpell);
	draw_caster_buttons(castSpell, type);
	put_spell_led_buttons(castSpell, type, former_spell);
	
	if(univ.party.help_received[7] == 0) {
		// TODO: Not sure if this initial draw is needed
//		cd_initial_draw(1098);
		give_help(7,8,castSpell);
	}
	
	castSpell.run();
	
	return cSpell::fromNum(type, castSpell.getResult<short>());
}


void print_spell_cast(eSpell spell,eSkill which)
//short which; // 0 - mage  1 - priest
{
	short spell_num = (which == eSkill::PRIEST_SPELLS ? int(spell) - 100 : int(spell));
	sprintf ((char *) c_line, "Spell: %s                  ",
			 (which == eSkill::MAGE_SPELLS) ? (char *) mage_s_name[spell_num] : (char *) priest_s_name[spell_num]);
	add_string_to_buf((char *) c_line);
}

short stat_adj(short pc_num,eSkill which) {
	short tr;
	
	tr = skill_bonus[univ.party[pc_num].skills[which]];
	if(which == eSkill::INTELLIGENCE) {
		if(univ.party[pc_num].traits[eTrait::MAGICALLY_APT])
			tr++;
		if(pc_has_abil_equip(pc_num,eItemAbil::INTELLIGENCE) < 24)
			tr++;
	}
	if(which == eSkill::STRENGTH) {
		if(univ.party[pc_num].traits[eTrait::STRENGTH])
			tr++;
		if(pc_has_abil_equip(pc_num,eItemAbil::STRENGTH) < 24)
			tr++;
	}
	if(which == eSkill::DEXTERITY) {
		if(pc_has_abil_equip(pc_num,eItemAbil::DEXTERITY) < 24)
			tr++;
	}
	return tr;
}

void start_town_targeting(eSpell s_num,short who_c,bool freebie) {
	overall_mode = MODE_TOWN_TARGET;
	town_spell = s_num;
	who_cast = who_c;
	spell_freebie = freebie;
}

void do_alchemy() ////
{
	static const eItemAbil ingred1_needed[20] = {
		eItemAbil::HOLLY,eItemAbil::COMFREY,eItemAbil::HOLLY,eItemAbil::COMFREY,eItemAbil::WORMGRASS,
		eItemAbil::NETTLE,eItemAbil::NETTLE,eItemAbil::WORMGRASS,eItemAbil::GRAYMOLD,eItemAbil::WORMGRASS,
		eItemAbil::GRAYMOLD,eItemAbil::ASPTONGUE,eItemAbil::GRAYMOLD,eItemAbil::MANDRAKE,eItemAbil::EMBERF,
		eItemAbil::MANDRAKE,eItemAbil::MANDRAKE,eItemAbil::NETTLE,eItemAbil::GRAYMOLD,eItemAbil::MANDRAKE,
	};
	static const eItemAbil ingred2_needed[20] = {
		eItemAbil::NONE,eItemAbil::NONE,eItemAbil::NONE,eItemAbil::WORMGRASS,eItemAbil::NONE,
		eItemAbil::NONE,eItemAbil::NONE,eItemAbil::NETTLE,eItemAbil::NONE,eItemAbil::ASPTONGUE,
		eItemAbil::HOLLY,eItemAbil::NONE,eItemAbil::COMFREY,eItemAbil::NONE,eItemAbil::NONE,
		eItemAbil::ASPTONGUE,eItemAbil::EMBERF,eItemAbil::EMBERF,eItemAbil::ASPTONGUE,eItemAbil::EMBERF,
	};
	static const short difficulty[20] = {
		1,1,1,3,3,
		4,5,5,7,9,
		9,10,12,12,9,
		14,19,10,16,20
	};
	static const short fail_chance[20] = {
		50,40,30,20,10,
		8,6,4,2,0,
		0,0,0,0,0,
		0,0,0,0,0
	};
	short which_p,which_item,which_item2,r1;
	short pc_num;
	cItemRec store_i('alch');// = {7,0, 0,0,0,1,0,0, 50,0,0,0,0, 0, 8,0, {0,0},"Potion","Potion",0,5,0,0};
	
//	{7,0,0,0,0,1,1,30,59,0,0,250,1,0,1,{0,0},"Graymold Salve","Potion"},
//	{7,0,0,0,0,1,1,30,13,0,0,250,1,0,1,{0,0},"Resurrection Balm","Potion"},
	
	static const short potion_abils[20] = {
		72,87,70,73,70,
		87,72,73,77,88,
		79,70,87,70,160,
		88,86,71,84,88
	};
	static const short potion_strs[20] = {
		2,2,2,2,4, 5,8,5,4,2,
		8,6,8,8,0, 5,2,8,5,8
	};
	static const short potion_val[20] = {
		40,60,15,50,50,
		180,200,100,150,100,
		200,150,300,400,100,
		300,500,175,250,500
	};
	
	pc_num = select_pc(1,0);
	if(pc_num == 6)
		return;
	
	which_p = alch_choice(pc_num);
	if(which_p < 20) {
		if(pc_has_space(pc_num) == 24) {
			add_string_to_buf("Alchemy: Can't carry another item.");
			return;
		}
		// TODO: Spread this out a bit, since there seems to be a chance of which_item2 not being initialized.
		if(((which_item = pc_has_abil(pc_num,ingred1_needed[which_p])) == 24) ||
			(ingred2_needed[which_p] != eItemAbil::NONE && (which_item2 = pc_has_abil(pc_num,ingred2_needed[which_p])) == 24)) {
			add_string_to_buf("Alchemy: Don't have ingredients.");
			return;
		}
		play_sound(8);
		remove_charge(pc_num,which_item);
		if(ingred2_needed[which_p] != eItemAbil::NONE)
			remove_charge(pc_num,which_item2);
		
		r1 = get_ran(1,1,100);
		if(r1 < fail_chance[univ.party[pc_num].skills[eSkill::ALCHEMY] - difficulty[which_p]]) {
			add_string_to_buf("Alchemy: Failed.               ");
			r1 = get_ran(1,0,1);
			play_sound(41 );
		}
		else {
			store_i.value = potion_val[which_p];
			store_i.ability_strength = potion_strs[which_p];
			store_i.ability = (eItemAbil) potion_abils[which_p];
			if(which_p == 8)
				store_i.magic_use_type = 2;
			store_i.full_name = alch_names_short[which_p];
			if(univ.party[pc_num].skills[eSkill::ALCHEMY] - difficulty[which_p] >= 5)
				store_i.charges++;
			if(univ.party[pc_num].skills[eSkill::ALCHEMY] - difficulty[which_p] >= 11)
				store_i.charges++;
			if(store_i.variety == eItemType::POTION)
				store_i.graphic_num += get_ran(1,0,2);
			if(!give_to_pc(pc_num,store_i,0)) {
				ASB("No room in inventory.");
				ASB("  Potion placed on floor.");
				place_item(store_i,univ.town.p_loc,true);
			}
			else add_string_to_buf("Alchemy: Successful.             ");
		}
		put_item_screen(stat_window,0);
	}
	
}

static bool alch_choice_event_filter(cDialog& me, std::string item_hit, eKeyMod) {
	if(item_hit == "help") {
		univ.party.help_received[20] = 0;
		give_help(20,21,me);
		return true;
	}
	if(item_hit == "cancel")
		me.setResult<short>(20);
	else {
		me.setResult<short>(item_hit[6] - '1');
	}
	me.toast(true);
	return true;
}

short alch_choice(short pc_num)
{
	short difficulty[20] = {1,1,1,3,3, 4,5,5,7,9, 9,10,12,12,9, 14,19,10,16,20};
	short i,store_alchemy_pc;
	char get_text[256];
	
	make_cursor_sword();
	
	store_alchemy_pc = pc_num;
	
	cDialog chooseAlchemy("pick-potion.xml");
	chooseAlchemy.attachClickHandlers(alch_choice_event_filter, {"cancel", "help"});
	for(i = 0; i < 20; i++) {
		std::string n = boost::lexical_cast<std::string>(i + 1);
		chooseAlchemy["label" + n].setText(alch_names[i]);
		chooseAlchemy["potion" + n].attachClickHandler(alch_choice_event_filter);
		if(univ.party[pc_num].skills[eSkill::ALCHEMY] < difficulty[i] || univ.party.alchemy[i] == 0)
			chooseAlchemy["potion" + n].hide();
	}
	sprintf((char *) get_text, "%s (skill %d)",
			univ.party[pc_num].name.c_str(),univ.party[pc_num].skills[eSkill::ALCHEMY]);
	chooseAlchemy["mixer"].setText(get_text);
	if(univ.party.help_received[20] == 0) {
		// TODO: I'm not sure if the initial draw is needed
//		cd_initial_draw(1047);
		give_help(20,21,chooseAlchemy);
	}
	
	chooseAlchemy.run();
	return chooseAlchemy.getResult<short>();
}

extern bool pc_gworld_loaded;
bool pick_pc_graphic(short pc_num,short mode,cDialog* parent)
// mode ... 0 - create  1 - created
{
	bool munch_pc_graphic = false;
	
	store_graphic_pc_num = pc_num;
	store_graphic_mode = mode;
	
	make_cursor_sword();
	
	if(!pc_gworld_loaded) {
		munch_pc_graphic = true;
		pc_gworld.loadFromImage(*ResMgr::get<ImageRsrc>("pcs"));
	}
	cPictChoice pcPic(0,35,PIC_PC,parent);
	// Customize it for this special case of choosing a PC graphic
	dynamic_cast<cPict&>(pcPic->getControl("mainpic")).setPict(7);
	pcPic->getControl("prompt").setText("Select a graphic for your PC:");
	pcPic->getControl("help").setText("Click button to left of graphic to select.");
	
	bool madeChoice = pcPic.show(univ.party[pc_num].which_graphic);
	if(mode == 0 && !madeChoice && univ.party[pc_num].main_status < eMainStatus::ABSENT)
		univ.party[pc_num].main_status = eMainStatus::ABSENT;
	else if(madeChoice)
		univ.party[pc_num].which_graphic = pcPic.getPicChosen();
	
	return madeChoice;
}

static bool pc_name_event_filter(cDialog& me, short store_train_pc) {
	std::string pcName = me["name"].getText();
	
	if(!isalpha(pcName[0])) {
		me["error"].setText("Must begin with a letter.");
	}
	else {
		// TODO: This was originally truncated to 18 characters; is that really necessary?
		univ.party[store_train_pc].name = pcName;
		me.toast(true);
	}
	return true;
}

bool pick_pc_name(short pc_num,cDialog* parent)
//town_num; // Will be 0 - 200 for town, 200 - 290 for outdoors
//short sign_type; // terrain type
{
	using namespace std::placeholders;
	make_cursor_sword();
	
	cDialog pcPickName("pick-pc-name.xml", parent);
	pcPickName["name"].setText(univ.party[pc_num].name);
	pcPickName["okay"].attachClickHandler(std::bind(pc_name_event_filter, _1, pc_num));
	
	pcPickName.run();
	
	return 1;
}

m_num_t pick_trapped_monst()
// ignore parent in Mac version
{
	short i;
	std::string sp;
	cMonster get_monst;
	
	make_cursor_sword();
	
	cChoiceDlog soulCrystal("soul-crystal.xml",{"cancel","pick1","pick2","pick3","pick4"});
	
	for(i = 0; i < 4; i++) {
		std::string n = boost::lexical_cast<std::string>(i + 1);
		if(univ.party.imprisoned_monst[i] == 0) {
			soulCrystal->getControl("pick" + n).hide();
		}
		else {
			sp = get_m_name(univ.party.imprisoned_monst[i]);
			soulCrystal->getControl("slot" + n).setText(sp);
			get_monst = scenario.scen_monsters[univ.party.imprisoned_monst[i]];
			soulCrystal->getControl("lvl" + n).setTextToNum(get_monst.level);
		}
	}
	
	
	std::string result = soulCrystal.show();
	
	if(result == "cancel")
		return 0;
	else return univ.party.imprisoned_monst[result[4] - '1'];
}


bool flying()
{
	if(PSD[SDF_PARTY_FLIGHT] == 0)
		return false;
	else return true;
}

void poison_pc(short which_pc,short how_much)
{
	short level;
	
	if(univ.party[which_pc].main_status == eMainStatus::ALIVE) {
		if((level = get_prot_level(which_pc,eItemAbil::POISON_PROTECTION)) > 0)
			how_much -= level / 2;
		if((level = get_prot_level(which_pc,eItemAbil::FULL_PROTECTION)) > 0)
			how_much -= level / 3;
		
		if(univ.party[which_pc].traits[eTrait::FRAIL] && how_much > 1)
			how_much++;
		if(univ.party[which_pc].traits[eTrait::FRAIL] && how_much == 1 && get_ran(1,0,1) == 0)
			how_much++;
		
		if(how_much > 0) {
			univ.party[which_pc].status[eStatus::POISON] = min(univ.party[which_pc].status[eStatus::POISON] + how_much,8);
			sprintf ((char *) c_line, "  %s poisoned.",
					 (char *) univ.party[which_pc].name.c_str());
			add_string_to_buf((char *) c_line);
			one_sound(17);
			give_help(33,0);
		}
	}
	put_pc_screen();
}

void poison_party(short how_much)
{
	short i;
	
	for(i = 0; i < 6; i++)
		poison_pc(i,how_much);
}
void affect_pc(short which_pc,eStatus type,short how_much)////
//type; // which status to affect
{
	
	if(univ.party[which_pc].main_status != eMainStatus::ALIVE)
		return;
	univ.party[which_pc].status[type] = minmax (-8,8,univ.party[which_pc].status[type] + how_much);
	// TODO: Original code also allowed poisoned weapon, poison, and sleep to be negative; should this be restored?
	// TODO: For certain other status effects it could make sense to allow negatives, eg magic resistance or dumbfounded
	if(type != eStatus::BLESS_CURSE && type != eStatus::HASTE_SLOW)
		univ.party[which_pc].status[type] = max(univ.party[which_pc].status[type],0);
	put_pc_screen();
}
void affect_party(eStatus type,short how_much)
//type; // which status to affect
{
	short i;
	
	for(i = 0; i < 6; i++)
		if(univ.party[i].main_status == eMainStatus::ALIVE)
			univ.party[i].status[type] = minmax (-8,8,univ.party[i].status[type] + how_much);
	put_pc_screen();
}

void void_sanctuary(short pc_num)
{
	if(univ.party[pc_num].status[eStatus::INVISIBLE] > 0) {
		add_string_to_buf("You become visible!");
		univ.party[pc_num].status[eStatus::INVISIBLE] = 0;
	}
}

void hit_party(short how_much,eDamageType damage_type)
{
	short i;
	bool dummy;
	
	for(i = 0; i < 6; i++)
		if(univ.party[i].main_status == eMainStatus::ALIVE)
			dummy = damage_pc(i,how_much,damage_type,eRace::UNKNOWN,0);
//			dummy = damage_pc(i,how_much,damage_type + 30);
	put_pc_screen();
}

void slay_party(eMainStatus mode)
{
	short i;
	
	boom_anim_active = false;
	for(i = 0; i < 6; i++)
		if(univ.party[i].main_status == eMainStatus::ALIVE)
			univ.party[i].main_status = mode;
	put_pc_screen();
}

bool damage_pc(short which_pc,short how_much,eDamageType damage_type,eRace type_of_attacker, short sound_type)
//short damage_type; // 0 - weapon   1 - fire   2 - poison   3 - general magic   4 - unblockable
// 5 - cold  6 - undead attack  7 - demon attack
// 10 - marked damage, from during anim mode ... no boom, and totally unblockable
// 30 + *   same as *, but no print
// 100s digit - sound data
{
	short i, r1,level;
	bool do_print = true;
	
	if(univ.party[which_pc].main_status != eMainStatus::ALIVE)
		return false;
	
	//sound_type = damage_type / 100;
	//damage_type = damage_type % 100;
	
	if(damage_type >= DAMAGE_NO_PRINT) {
		do_print = false;
		damage_type -= DAMAGE_NO_PRINT;
	}
	
	if(sound_type == 0) {
		if((damage_type == 1) || (damage_type == 4) )
			sound_type = 5;
		if 	(damage_type == 3)
			sound_type = 12;
		if 	(damage_type == 5)
			sound_type = 7;
		if 	(damage_type == 2)
			sound_type = 11;
	}
	
	// armor
	if((damage_type == 0) || (damage_type == 6) ||(damage_type == 7)) {
		how_much -= minmax(-5,5,univ.party[which_pc].status[eStatus::BLESS_CURSE]);
		for(i = 0; i < 24; i++)
			if((univ.party[which_pc].items[i].variety != eItemType::NO_ITEM) && (univ.party[which_pc].equip[i])) {
				if(isArmourType(univ.party[which_pc].items[i].variety)) {
					r1 = get_ran(1,1,univ.party[which_pc].items[i].item_level);
					how_much -= r1;
					
					// bonus for magical items
					if(univ.party[which_pc].items[i].bonus > 0) {
						r1 = get_ran(1,1,univ.party[which_pc].items[i].bonus);
						how_much -= r1;
						how_much -= univ.party[which_pc].items[i].bonus / 2;
					}
					if(univ.party[which_pc].items[i].bonus < 0) {
						how_much = how_much - univ.party[which_pc].items[i].bonus;
					}
					r1 = get_ran(1,1,100);
					if(r1 < hit_chance[univ.party[which_pc].skills[eSkill::DEFENSE]] - 20)
						how_much -= 1;
				}
				if(univ.party[which_pc].items[i].protection > 0) {
					r1 = get_ran(1,1,univ.party[which_pc].items[i].protection);
					how_much -= r1;
				}
				if(univ.party[which_pc].items[i].protection < 0) {
					r1 = get_ran(1,1,-1 * univ.party[which_pc].items[i].protection);
					how_much += r1;
				}
			}
	}
	
	// parry
	if((damage_type < 2) && (univ.party[which_pc].parry < 100))
		how_much -= univ.party[which_pc].parry / 4;
	
	
	if(damage_type != 10) {
		if(PSD[SDF_EASY_MODE] > 0)
			how_much -= 3;
		// toughness
		if(univ.party[which_pc].traits[eTrait::TOUGHNESS])
			how_much--;
		// luck
		if(get_ran(1,1,100) < 2 * (hit_chance[univ.party[which_pc].skills[eSkill::LUCK]] - 20))
			how_much -= 1;
	}
	
	if((damage_type == DAMAGE_WEAPON) && ((level = get_prot_level(which_pc,eItemAbil::PROTECTION)) > 0))
		how_much = how_much - level;
	if((damage_type == DAMAGE_UNDEAD) && ((level = get_prot_level(which_pc,eItemAbil::PROTECT_FROM_UNDEAD)) > 0))
		how_much = how_much / ((level >= 7) ? 4 : 2);
	if((damage_type == DAMAGE_DEMON) && ((level = get_prot_level(which_pc,eItemAbil::PROTECT_FROM_DEMONS)) > 0))
		how_much = how_much / ((level >= 7) ? 4 : 2);
	if((type_of_attacker == eRace::HUMANOID) && ((level = get_prot_level(which_pc,eItemAbil::PROTECT_FROM_HUMANOIDS)) > 0))
		how_much = how_much / ((level >= 7) ? 4 : 2);
	// TODO: Should sliths be counted as reptiles?
	if((type_of_attacker == eRace::REPTILE) && ((level = get_prot_level(which_pc,eItemAbil::PROTECT_FROM_REPTILES)) > 0))
		how_much = how_much / ((level >= 7) ? 4 : 2);
	if((type_of_attacker == eRace::GIANT) && ((level = get_prot_level(which_pc,eItemAbil::PROTECT_FROM_GIANTS)) > 0))
		how_much = how_much / ((level >= 7) ? 4 : 2);
	
	
	// invuln
	if(univ.party[which_pc].status[eStatus::INVULNERABLE] > 0)
		how_much = 0;
	
	// magic resistance
	if(damage_type == DAMAGE_MAGIC && ((level = get_prot_level(which_pc,eItemAbil::MAGIC_PROTECTION)) > 0))
		how_much = how_much / ((level >= 7) ? 4 : 2);
	
	// Mag. res helps w. fire and cold
	if(((damage_type == 1) || (damage_type == 5)) &&
		univ.party[which_pc].status[eStatus::MAGIC_RESISTANCE] > 0)
		how_much = how_much / 2;
	
	// fire res.
	if(damage_type == DAMAGE_FIRE && ((level = get_prot_level(which_pc,eItemAbil::FIRE_PROTECTION)) > 0))
		how_much = how_much / ((level >= 7) ? 4 : 2);
	
	// cold res.
	if(damage_type == DAMAGE_COLD && ((level = get_prot_level(which_pc,eItemAbil::COLD_PROTECTION)) > 0))
		how_much = how_much / ((level >= 7) ? 4 : 2);
	
	// major resistance
	if((damage_type == DAMAGE_FIRE || damage_type == DAMAGE_POISON || damage_type == DAMAGE_MAGIC || damage_type == DAMAGE_COLD)
	   && ((level = get_prot_level(which_pc,eItemAbil::FULL_PROTECTION)) > 0))
		how_much = how_much / ((level >= 7) ? 4 : 2);
	
	if(boom_anim_active) {
		if(how_much < 0)
			how_much = 0;
		univ.party[which_pc].marked_damage += how_much;
		if(is_town())
			add_explosion(univ.town.p_loc,how_much,0,(damage_type > 2) ? 2 : 0,0,0);
		else add_explosion(univ.party[which_pc].combat_pos,how_much,0,(damage_type > 2) ? 2 : 0,0,0);
		//sprintf ((char *) c_line, "  %s takes %d. ",(char *) univ.party[which_pc].name, how_much);
		//if(do_print)
		//	add_string_to_buf((char *) c_line);
		if(how_much == 0)
			return false;
		else return true;
	}
	
	if(how_much <= 0) {
		if((damage_type == 0) || (damage_type == 6) || (damage_type == 7))
			play_sound(2);
		add_string_to_buf ("  No damage.");
		return false;
	}
	else {
		// if asleep, get bonus
		if(univ.party[which_pc].status[eStatus::ASLEEP] > 0)
			univ.party[which_pc].status[eStatus::ASLEEP]--;
		
		sprintf ((char *) c_line, "  %s takes %d. ",(char *) univ.party[which_pc].name.c_str(), how_much);
		if(do_print)
			add_string_to_buf((char *) c_line);
		if(damage_type != 10) {
			if(is_combat())
				boom_space(univ.party[which_pc].combat_pos,overall_mode,boom_gr[damage_type],how_much,sound_type);
			else if(is_town())
				boom_space(univ.town.p_loc,overall_mode,boom_gr[damage_type],how_much,sound_type);
			else boom_space(univ.town.p_loc,100,boom_gr[damage_type],how_much,sound_type);
		}
		// TODO: When outdoors it flushed only key events, not mouse events. Why?
		flushingInput = true;
	}
	
	univ.party.total_dam_taken += how_much;
	
	if(univ.party[which_pc].cur_health >= how_much)
		univ.party[which_pc].cur_health = univ.party[which_pc].cur_health - how_much;
	else if(univ.party[which_pc].cur_health > 0)
		univ.party[which_pc].cur_health = 0;
	else // Check if PC can die
		if(how_much > 25) {
			sprintf ((char *) c_line, "  %s is obliterated.  ",(char *) univ.party[which_pc].name.c_str());
			add_string_to_buf((char *) c_line);
			kill_pc(which_pc, eMainStatus::DUST);
		}
		else {
			sprintf ((char *) c_line, "  %s is killed.",(char *) univ.party[which_pc].name.c_str());
			add_string_to_buf((char *) c_line);
			kill_pc(which_pc,eMainStatus::DEAD);
		}
	if(univ.party[which_pc].cur_health == 0 && univ.party[which_pc].main_status == eMainStatus::ALIVE)
		play_sound(3);
	put_pc_screen();
	return true;
}

void kill_pc(short which_pc,eMainStatus type)
{
	short i = 24;
	bool dummy,no_save = false;
	location item_loc;
	
	if(isSplit(type)) {
		type -= eMainStatus::SPLIT;
		no_save = true;
	}
	
	if(type != eMainStatus::STONE)
		i = pc_has_abil_equip(which_pc,eItemAbil::LIFE_SAVING);
	
	if(!no_save && type != eMainStatus::ABSENT && univ.party[which_pc].skills[eSkill::LUCK] > 0 &&
	   get_ran(1,1,100) < hit_chance[univ.party[which_pc].skills[eSkill::LUCK]]) {
		add_string_to_buf("  But you luck out!          ");
		univ.party[which_pc].cur_health = 0;
	}
	else if(i == 24 || type == eMainStatus::ABSENT) {
		if(combat_active_pc == which_pc)
			combat_active_pc = 6;
		
		for(i = 0; i < 24; i++)
			univ.party[which_pc].equip[i] = false;
		
		item_loc = (overall_mode >= MODE_COMBAT) ? univ.party[which_pc].combat_pos : univ.town.p_loc;
		
		if(type == eMainStatus::DEAD)
			univ.town.set_lg_blood(item_loc.x,item_loc.y,true);
		else if(type == eMainStatus::DUST)
			univ.town.set_ash(item_loc.x,item_loc.y,true);
		
		if(overall_mode != MODE_OUTDOORS)
			for(i = 0; i < 24; i++)
				if(univ.party[which_pc].items[i].variety != eItemType::NO_ITEM) {
					dummy = place_item(univ.party[which_pc].items[i],item_loc,true);
					univ.party[which_pc].items[i].variety = eItemType::NO_ITEM;
				}
		if(type == eMainStatus::DEAD || type == eMainStatus::DUST)
			play_sound(21);
		univ.party[which_pc].main_status = type;
		univ.party[which_pc].ap = 0;
	}
	else {
		add_string_to_buf("  Life saved!              ");
		take_item(which_pc,i);
		heal_pc(which_pc,200);
	}
	if(univ.party[current_pc].main_status != eMainStatus::ALIVE)
		current_pc = first_active_pc();
	put_pc_screen();
	set_stat_window(current_pc);
}

void set_pc_moves()
{
	short i,r,i_level;
	
	for(i = 0; i < 6; i++)
		if(univ.party[i].main_status != eMainStatus::ALIVE)
			univ.party[i].ap = 0;
		else {
			univ.party[i].ap = univ.party[i].traits[eTrait::SLUGGISH] ? 3 : 4;
			r = get_encumberance(i);
			univ.party[i].ap = minmax(1,8,univ.party[i].ap - (r / 3));
			
			if((i_level = get_prot_level(i,eItemAbil::SPEED)) > 0)
				univ.party[i].ap += i_level / 7 + 1;
			if((i_level = get_prot_level(i,eItemAbil::SLOW_WEARER)) > 0)
				univ.party[i].ap -= i_level / 5;
			
			if(univ.party[i].status[eStatus::HASTE_SLOW] < 0 && univ.party.age % 2 == 1) // slowed?
				univ.party[i].ap = 0;
			else { // do webs
				univ.party[i].ap = max(0,univ.party[i].ap - univ.party[i].status[eStatus::WEBS] / 2);
				if(univ.party[i].ap == 0) {
					sprintf((char *) c_line,"%s must clean webs.",univ.party[i].name.c_str());
					add_string_to_buf((char *) c_line);
					univ.party[i].status[eStatus::WEBS] = max(0,univ.party[i].status[eStatus::WEBS] - 3);
				}
			}
			if(univ.party[i].status[eStatus::HASTE_SLOW] > 7)
				univ.party[i].ap = univ.party[i].ap * 3;
			else if(univ.party[i].status[eStatus::HASTE_SLOW] > 0)
				univ.party[i].ap = univ.party[i].ap * 2;
			if(univ.party[i].status[eStatus::ASLEEP] > 0 || univ.party[i].status[eStatus::PARALYZED] > 0)
				univ.party[i].ap = 0;
			
		}
	
}

void take_ap(short num)
{
	univ.party[current_pc].ap = max(0,univ.party[current_pc].ap - num);
}

short trait_present(eTrait which_trait) {
	short i,ret = 0;
	for(i = 0; i < 6; i++)
		if(univ.party[i].main_status == eMainStatus::ALIVE && univ.party[i].traits[which_trait] > 0)
			ret += 1;
	return ret;
}

short race_present(eRace which_race) {
	short i,ret = 0;
	for(i = 0; i < 6; i++)
		if(univ.party[i].main_status == eMainStatus::ALIVE && univ.party[i].race == which_race)
			ret += 1;
	return ret;
}

short wilderness_lore_present() {
	// TODO: Add contional statement to choose between these
	// (Probably requires something added to terrain types to specify that it's cave/surface wilderness.)
	return trait_present(eTrait::CAVE_LORE); // Cave Lore
	return trait_present(eTrait::WOODSMAN); // Woodsman
}

short party_size(bool only_living) {
	short num_pcs = 0;
	for(short i = 0; i < 6; i++) {
		if(!only_living) {
			if(univ.party[i].main_status != eMainStatus::ABSENT)
				num_pcs++;
		}
		else {
			if(univ.party[i].main_status == eMainStatus::ALIVE)
				num_pcs++;		
		}
	}
	
	return num_pcs;
	
}
