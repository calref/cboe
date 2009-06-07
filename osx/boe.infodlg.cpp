#include <cstdio>

//#include "item.h"

#include "boe.global.h"

#include "classes.h"

#include "boe.graphics.h"
#include "boe.newgraph.h"
#include "boe.items.h"
#include <cstring>
#include "boe.monster.h"
#include "dlgtool.h"
#include "dlgconsts.h"
#include "boe.party.h"
#include "boe.fields.h"
#include "boe.locutils.h"
#include "boe.text.h"
#include "soundtool.h"
#include "boe.infodlg.h"
#include "boe.fileio.h"
#include "boe.main.h"
#include "mathutil.h"
#include "dlgutil.h"
#include "fileio.h"

short mage_spell_pos = 0,priest_spell_pos = 0,skill_pos = 0;
cPlayer *store_pc;
cCreature *store_m;
short store_trait_mode,store_item_pc,store_pc_num;
cItemRec store_i;

extern short spell_w_cast[2][62];
extern short spell_level[62];
extern short skill_cost[20];
extern short skill_max[20];
extern short skill_g_cost[20];
extern short which_pc_displayed;
//extern party_record_type	party;
extern short mage_range[66],priest_range[66];
extern short spell_cost[2][62],cur_town_talk_loaded;
//extern current_town_type univ.town;
extern bool in_startup_mode,give_intro_hint;
extern WindowPtr mainPtr;
extern short on_monst_menu[256];
//extern big_tr_type t_d;
extern ModalFilterUPP main_dialog_UPP;
extern location tinraya_portculli[12];
//extern piles_of_stuff_dumping_type *data_store;

bool full_roster = false;

extern short dest_personalities[40];
extern location dest_locs[40];

extern short dest_personalities[40];
location source_locs[6] = {loc(2,9),loc(0,6),loc(3,6),loc(3,4),loc(6,2),loc(0,0)};
//extern current_town_type univ.town;
extern location dest_locs[40] ;
extern char *alch_names[];
extern cScenario scenario;
extern cUniverse univ;

// Displaying string vars
short store_str1a;
short store_str1b;
short store_str2a;
short store_str2b;
short store_which_string_dlog;
short store_page_on,store_num_i;
extern short store_str_label_1,store_str_label_2,store_str_label_1b,store_str_label_2b;

// Misc dialog vars
short store_display_mode,store_displayed_item,position,num_entries,store_help_mode;
unsigned short cur_entry;
cCreature hold_m;
	
void put_spell_info()
{
	Str255 store_text;
	short pos,res,ran;


	pos = (store_display_mode == 0) ? mage_spell_pos : priest_spell_pos;
	res = (store_display_mode == 0) ? 7 : 8;
	ran = (store_display_mode == 0) ? mage_range[pos] : priest_range[pos];
	
	get_str (store_text, res, pos * 2 + 1);
	cd_set_item_text(1096,4,(char *) store_text);

	if (spell_cost[store_display_mode][pos] > 0)
		sprintf((char *) store_text, "%d/%d",spell_level[pos],spell_cost[store_display_mode][pos]);
		else sprintf((char *) store_text, "%d/?",spell_level[pos]);
	cd_set_item_text(1096,5,(char *) store_text);

	if (ran == 0) {
		cd_set_item_text(1096,6,"");
		}
		else cd_set_item_num(1096,6,ran);
	get_str (store_text, res, pos * 2 + 2);

	cd_set_item_text(1096,7,(char *) store_text);
	get_str (store_text, 11, 1 + spell_w_cast[store_display_mode][pos]);
	cd_set_item_text(1096,11,(char *) store_text);
}


void display_spells_event_filter (short item_hit)
{
	short store;
			switch (item_hit) {
				case 1: case 8:
					toast_dialog();
					break;

				case 9: case 10:
					store = (store_display_mode == 0) ? mage_spell_pos : priest_spell_pos;
					if (item_hit == 9) {
						store = (store == 0) ? 61 : store - 1;
						}
						else {
							store = (store + 1) % 62;
							}
					if (store_display_mode == 0)
						mage_spell_pos = store;
						else priest_spell_pos = store;
					put_spell_info();
					break;
				}
}
void display_spells(short mode,short force_spell,short parent_num)
//short mode; // 0 - mage  1 - priest
//short force_spell; // if 100, ignore
{
	short item_hit;

	store_display_mode = mode;
	if (force_spell < 100) {
		if (mode == 0)
			mage_spell_pos = force_spell;
			else priest_spell_pos = force_spell;
		}

	make_cursor_sword();

	cd_create_dialog_parent_num(1096,parent_num);

      cd_set_pict(1096,18,14 + mode,PICT_DLG);
		put_spell_info();
	if (mode == 0)
		csit(1096,3,"Mage Spells");
	else csit(1096,3,"Priest Spells");
	
	item_hit = cd_run_dialog();		
	cd_kill_dialog(1096);

}

void put_skill_info()
{
	Str255 store_text;
	short pos;

	pos = skill_pos;
	
	get_str(store_text,9,pos * 2 + 1);
	cd_set_item_text(1097,3,(char *) store_text);
	cd_set_item_num(1097,4,skill_cost[pos]);
	cd_set_item_num(1097,5,skill_g_cost[pos]);
	cd_set_item_num(1097,6,skill_max[pos]);

	get_str (store_text, 9, pos * 2 + 2);
	cd_set_item_text(1097,7,(char *) store_text);
	get_str (store_text, 12, 1 + pos);
	cd_set_item_text(1097,8,(char *) store_text);
}


void display_skills_event_filter (short item_hit)
{
			switch (item_hit) {
				case 1: case 11:
					toast_dialog();
					break;

				case 10: case 9:
					if (item_hit == 9) {
						skill_pos = (skill_pos == 0) ? 18 : skill_pos - 1;
						}
						else {
							skill_pos = (skill_pos + 1) % 19;
							}
					put_skill_info();
					break;
				}
}

void display_skills(short force_skill,short parent)
{

	short item_hit;

	if (force_skill < 100)
		skill_pos = force_skill;
	if (skill_pos < 0)
		skill_pos = 0;

	make_cursor_sword();

	cd_create_dialog_parent_num(1097,parent);

	put_skill_info();
	
	item_hit = cd_run_dialog();	
	cd_kill_dialog(1097);

}

void put_pc_graphics()
{
	short i;

	for (i = 3; i < 65; i++) {
		if (((store_trait_mode == 0) && (univ.party[which_pc_displayed].mage_spells[i - 3] == true)) ||
		 ((store_trait_mode == 1) && (univ.party[which_pc_displayed].priest_spells[i - 3] == true)))
			cd_set_led(991,i,1);
			else cd_set_led(991,i,0);
		}

	cd_set_item_text(991,69,univ.party[which_pc_displayed].name.c_str());
}

void display_pc_event_filter (short item_hit)
{
	short pc_num;

	pc_num = which_pc_displayed;
			switch (item_hit) {
				case 1: case 65:
					toast_dialog();
					break;

				case 66:
					do {
						pc_num = (pc_num == 0) ? 5 : pc_num - 1;
						} while (univ.party[pc_num].main_status == 0);
					which_pc_displayed = pc_num;
					put_pc_graphics();
					break;
				case 67:
					do {
						pc_num = (pc_num == 5) ? 0 : pc_num + 1;
						} while (univ.party[pc_num].main_status == 0);
					which_pc_displayed = pc_num;
					put_pc_graphics();	
					break;
					
				case 100:
					break;

				}
}

void display_pc(short pc_num,short mode,short parent)
{
	short i,item_hit;
	Str255 label_str;
	
	if (univ.party[pc_num].main_status == 0) {
		for (pc_num = 0; pc_num < 6; pc_num++)
			if (univ.party[pc_num].main_status == 1)
				break;
		}
	which_pc_displayed = pc_num;
	store_trait_mode = mode;

	make_cursor_sword();

	cd_create_dialog_parent_num(991,parent);

	for (i = 3; i < 65; i++) {
		get_str(label_str,(mode == 0) ? 7 : 8,(i - 3) * 2 + 1);
		cd_add_label(991,i,(char *)label_str,46);
		}
	put_pc_graphics();

	cd_set_pict(991,2,14 + mode,PICT_DLG);
	
	item_hit = cd_run_dialog();	
	cd_kill_dialog(991);
}

void put_item_info(short pc,short item)////
{
	char store_text[256];
	Str255 desc_str;
	short i;	
	cItemRec s_i;
	char *item_types[] = {"","1-Handed weapon","2-Handed weapon","","Bow","Arrows","Thrown missile",
			"Potion/Magic Item","Scroll/Magic Item","Wand","Tool","","Shield","Armor","Helm",
			"Gloves","Shield","Boots","Ring","Necklace",
			"Weapon Poison","Gem, Stone, Etc.","Pants","Crossbow","Bolts","Missile Weapon"};

		
	s_i = store_i;
	
	csp(998,1,0,PICT_BLANK);
	if (s_i.graphic_num >= 1000) // was 150
		csp(998,1,s_i.graphic_num - 1000,PICT_CUSTOM + PICT_ITEM);
		else csp(998,1,s_i.graphic_num,PICT_ITEM);
		
	// id? magic?
	if (store_i.is_magic() && store_i.is_ident())
		cd_set_led(998,17,1);
		else cd_set_led(998,17,0);
	if (store_i.is_ident())
		cd_set_led(998,16,1);
		else cd_set_led(998,16,0);
	cd_set_item_text(998,4,	item_types[s_i.variety]);
	
	// Clear fields
	for (i = 5; i < 13; i++) {
		cd_set_item_text(998,i,	"");
		}
			

	if (!s_i.is_ident()) {
			cd_set_item_text(998,3,	s_i.name.c_str());
			return;
		}	
			
	cd_set_item_text(998,3,	s_i.full_name.c_str());
	i = s_i.item_weight();
	cd_set_item_num(998,20,i);
	
	cd_set_item_num(998,5,(s_i.charges > 0) ? s_i.value * s_i.charges : s_i.value);

	if (s_i.ability > 0) {////
		if (s_i.item_properties & 32) { // Concealed ability
			cd_set_item_text(998,12,"???");
		} else {
			get_str(desc_str,23,s_i.ability + 1);
			cd_set_item_text(998,12,(char *) desc_str);
		}
	}	
	if (s_i.charges > 0)
		cd_set_item_num(998,10,s_i.charges);
	if (s_i.protection > 0)	
		cd_set_item_num(998,8,s_i.protection);

	switch (s_i.variety) {
		case ITEM_TYPE_ONE_HANDED:
		case ITEM_TYPE_TWO_HANDED:
			cd_set_item_num(998,6,s_i.item_level);
			cd_set_item_num(998,7,s_i.bonus);
		
			switch (s_i.type) {
				case ITEM_EDGED:
					sprintf((char *) store_text, "Edged weapon");
					break;
				case ITEM_BASHING:
					sprintf((char *) store_text, "Bashing weapon");
					break;
				case ITEM_POLE:
					sprintf((char *) store_text, "Pole weapon");
					break;
				case ITEM_NOT_MELEE:
				default:
					sprintf((char*)store_text, "Error weapon"); // should never be reached
				}
			if (s_i.ability == 0)
				cd_set_item_text(998,12,store_text);
			break;
		case ITEM_TYPE_BOW:
		case ITEM_TYPE_CROSSBOW:
			cd_set_item_num(998,6,s_i.item_level);
			cd_set_item_num(998,7,s_i.bonus);
			break;
		case ITEM_TYPE_ARROW:
		case ITEM_TYPE_THROWN_MISSILE:
		case ITEM_TYPE_BOLTS:
		case ITEM_TYPE_MISSILE_NO_AMMO:
			cd_set_item_num(998,6,s_i.item_level);
			cd_set_item_num(998,7,s_i.bonus);	
			break;
		case ITEM_TYPE_POTION:
		case ITEM_TYPE_RING:
		case ITEM_TYPE_SCROLL: // TODO: Does this make sense for a scroll, though?
		case ITEM_TYPE_TOOL: // and what about for a tool?
		case ITEM_TYPE_WAND: // and a wand? Maybe showing ability strength would be better...
		case ITEM_TYPE_NECKLACE: // TODO: This doesn't seem right for a necklace...
			cd_set_item_num(998,11,s_i.item_level);
			break;
		case ITEM_TYPE_SHIELD:
		case ITEM_TYPE_ARMOR:
		case ITEM_TYPE_HELM:
		case ITEM_TYPE_GLOVES:
		case ITEM_TYPE_SHIELD_2:
		case ITEM_TYPE_BOOTS: // TODO: Should this also check ITEM_TYPE_PANTS?
			cd_set_item_num(998,7,s_i.bonus + s_i.protection);	
			cd_set_item_num(998,8,s_i.item_level);	
			cd_set_item_num(998,9,s_i.awkward);
			break;	
		case ITEM_TYPE_WEAPON_POISON:
			cd_set_item_num(998,11,s_i.item_level);
			break;
		default:
			// no item, gold, food, non-use, unused 1 and 2: do nothing
			break;
		}	

}

void display_pc_item_event_filter (short item_hit)
{
	short item,pc_num;
	
	item = store_displayed_item;
	pc_num = store_item_pc;
	
			switch (item_hit) {
				case 1: case 13:
					toast_dialog();
					break;
				
				case 14:
					do {
						item = (item == 0) ? 23 : item - 1;
						} while (univ.party[pc_num].items[item].variety == 0);
					store_displayed_item = item;
					store_i = univ.party[pc_num].items[item];
					put_item_info(pc_num,item);
					break;
				case 15:
					do {
						item = (item == 23) ? 0 : item + 1;
						} while (univ.party[pc_num].items[item].variety == 0);
					store_displayed_item = item;
					store_i = univ.party[pc_num].items[item];
					put_item_info(pc_num,item);
					break;

				case 100:
					break;
					
				}	
}

void display_pc_item(short pc_num,short item,cItemRec si,short parent)
{
	short item_hit;

		store_item_pc = pc_num;
		if (pc_num == 6)
			store_i = si;
			else store_i = univ.party[pc_num].items[item];
	store_displayed_item = item;
	make_cursor_sword();

	if (cd_create_dialog_parent_num(998,parent) < 0)
		return;

		if (store_item_pc >= 6) {
			cd_activate_item(998,14,0);
			cd_activate_item(998,15,0);
			}
	cd_add_label(998,3,"Name:",1026);
	cd_add_label(998,4,"Type:",1026);
	cd_add_label(998,5,"Value:",1020);
	cd_add_label(998,6,"Damage:",1026);
	cd_add_label(998,7,"Bonus:",1024);
	cd_add_label(998,8,"Defend:",1025);
	cd_add_label(998,9,"Encumb.:",1029);
	cd_add_label(998,10,"Uses:",1019);
	cd_add_label(998,11,"Item Level:",1037);
	cd_add_label(998,12,"Ability:",1026);
	cd_add_label(998,16,"ID?",1013);
	cd_add_label(998,17,"Magic?",1022);
	cd_add_label(998,20 ,"Weight",1027);

	cd_activate_item(998,18,0);
	cd_activate_item(998,19,0);

	put_item_info(pc_num,item);
	
	item_hit = cd_run_dialog();
	cd_kill_dialog(998);

}


void put_monst_info()////
{
	string store_text;
	Str255 str;
	short abil,i;	
	
	if ( store_m->spec_skill == MONSTER_INVISIBLE) 
		cd_set_pict(999,4,400,PICT_MONST);// TODO: should probably be PICT_BLANK?
		else if (store_m->picture_num < 1000)
			cd_set_pict(999,4,store_m->picture_num,PICT_MONST);
		else {
			short type_g = PICT_CUSTOM + PICT_MONST;
			short size_g = store_m->picture_num / 1000;
			switch(size_g){
				case 2:
					type_g += PICT_WIDE_MONSTER;
					break;
				case 3:
					type_g += PICT_TALL_MONSTER;
					break;
				case 4:
					type_g += PICT_WIDE_MONSTER + PICT_TALL_MONSTER;
					break;
			}
			cd_set_pict(999,4,(store_m->picture_num % 1000), type_g);
		}
		
	store_text = get_m_name(store_m->number);
	cd_set_item_text(999,5,store_text.c_str());
	// Clear fields
	for (i = 6; i < 20; i++) {
		cd_set_item_text(999,i,"");
		}

	abil = store_m->spec_skill;
	get_str(str,20,abil + 1);
	cd_set_item_text(999,19,(char *) str);
	get_str(str,20,store_m->radiate_1 + 50);
	cd_set_item_text(999,31,(char *) str);
	
	for (i = 0; i < 3; i++)
		if (store_m->a[i] > 0) {
			std::ostringstream sout(store_text);
			sout << store_m->a[i] / 100 + 1 << 'd' << store_m->a[i] % 100;
			store_text = sout.str();

			cd_set_item_text(999,13 + i,store_text.c_str());
			}				
	cd_set_item_num(999,6,store_m->level);
	cd_set_item_num(999,7,store_m->health);
	cd_set_item_num(999,8,store_m->mp);
	cd_set_item_num(999,9,store_m->armor);
	cd_set_item_num(999,10,store_m->skill);
	cd_set_item_num(999,11,store_m->morale);
	cd_set_item_num(999,12,store_m->speed);
	cd_set_item_num(999,16,store_m->mu);
	cd_set_item_num(999,17,store_m->cl);
	cd_set_item_num(999,18,store_m->poison);
	// 2140 - lit  2141 - dark
	// immunities
	for (i = 0; i < 8; i++)
		if (store_m->immunities & (char)(s_pow(2,i)))
			cd_set_led(999,20 + i,1);
			else cd_set_led(999,20 + i,0);


	}


void display_monst_event_filter (short item_hit)
{
	short i,dummy = 0;
	
			switch (item_hit) {
				case 1:  case 3:
					toast_dialog();
					break;

				case 28:
					if (position == 0) {
						for (i = 255; on_monst_menu[i] < 0 && i > 0; i--)
							dummy++;
						position = i;
						}
						else position--;
						
					if (on_monst_menu[position] < 0)
						position = 0;
					store_m->number = on_monst_menu[position];
					*store_m = *store_m; // to fill in fields that wouldn't otherwise be filled in; replaces return_monster_template
					put_monst_info();
					break;
				case 29:
					position++;
					if (on_monst_menu[position] < 0)
						position = 0;
					store_m->number = on_monst_menu[position];
					*store_m = *store_m; // no, this is not redundant
					put_monst_info();
					break;

				case 100:
					break;

				}
}

void display_monst(short array_pos,cCreature *which_m,short mode)
//creature_data_type *which_m; // if NULL, show full roster
//short mode; // if 1, full roster, else use monster from storwhich_me_m
{
	
	short item_hit;
	
	position = array_pos;
	full_roster = false;
	if (mode == 1) {
		full_roster = true;
		store_m = &hold_m;
		store_m->number = on_monst_menu[array_pos];
		*store_m = *store_m; // yes, this DOES do something
		}
		else {
			hold_m = *which_m;
			store_m = which_m;
			}

	make_cursor_sword();

	cd_create_dialog(999,mainPtr);

	if (full_roster == false) {
		cd_activate_item(999,28,0);
		cd_activate_item(999,29,0);
		}
	cd_add_label(999,5,"Name",1026);
	cd_add_label(999,6,"Level",1021);
	cd_add_label(999,7,"Health",1024);
	cd_add_label(999,8,"Magic Pts.",1032);
	cd_add_label(999,9,"Armor",1023);
	cd_add_label(999,10,"Skill",1018);
	cd_add_label(999,11,"Morale",1023);
	cd_add_label(999,12,"Speed",1019);
	cd_add_label(999,13,"Att #1",1026);
	cd_add_label(999,14,"Att #2",1023);
	cd_add_label(999,15,"Att #3",1022);
	cd_add_label(999,16,"Mage L.",1034);
	cd_add_label(999,17,"Priest L.",1030);
	cd_add_label(999,18,"Poison",1023);
	cd_add_label(999,19,"Ability 1",1029);
	cd_add_label(999,31,"Ability 2",1029);
	cd_add_label(999,20,"Magic Resistant",45);
	cd_add_label(999,21,"Immune To Magic",45);
	cd_add_label(999,22,"Fire Resistant",45);
	cd_add_label(999,23,"Immune To Fire",45);
	cd_add_label(999,24,"Cold Resistant",45);
	cd_add_label(999,25,"Immune To Cold",45);
	cd_add_label(999,26,"Poison Resistant",45);
	cd_add_label(999,27,"Immune To Poison",45);
	put_monst_info();
	
	item_hit = cd_run_dialog();
	cd_kill_dialog(999);
}


void display_help_event_filter (short item_hit)
{
	Str255 get_text;
	
			switch (item_hit) {
				case 1: case 3:
					toast_dialog();
					break;

				case 4: case 5:
					if (item_hit == 4) 
						cur_entry = (cur_entry == 3) ? num_entries + 2 : cur_entry - 1;
						else cur_entry = (cur_entry == num_entries + 2) ? 3 : cur_entry + 1;
					get_str (get_text, 25 + store_help_mode, cur_entry);
					cd_set_item_text(997,7,(char *) get_text);
					break;
				}
}

void display_help(short mode,short parent)
{
	Str255 get_text;
   long get_val;
   short item_hit;
	
	store_help_mode = mode;
	cur_entry = 3;

	make_cursor_sword();

	cd_create_dialog_parent_num(997,parent);

		get_str (get_text, 25 + mode, 1);
		csit( 997,6,(char *) get_text);
		GetIndString (get_text, 25 + mode, 2);
		StringToNum(get_text,&get_val);
		num_entries = (short) get_val;
		get_str (get_text, 25 + mode, cur_entry);
		csit( 997,7,(char *) get_text);
	
	
	item_hit = cd_run_dialog();	
	cd_kill_dialog(997);

}



void display_alchemy_event_filter (short item_hit)
{
			switch (item_hit) {
				case 1: case 3:
					toast_dialog();
					break;

				}
}

void display_alchemy()
{
	short i,item_hit;


	make_cursor_sword();

	cd_create_dialog_parent_num(996,1019);


	for (i = 0; i < 20; i++) {
		cd_add_label(996,i + 4,alch_names[i],1083);
		if (univ.party.alchemy[i] > 0)
			cd_set_led(996,i + 4,1);
			else cd_set_led(996,i + 4,0);
		}
	
	item_hit = cd_run_dialog();		
	cd_kill_dialog(996);
	untoast_dialog();

}


void display_traits_graphics()
{
	short i,store;

	for (i = 0; i < 3; i++) {
		cd_set_led(1013,4 + i,(store_pc->race == i) ? 1 : 0);
		}
	for (i = 0; i < 10; i++) {
		cd_set_led(1013,7 + i,(store_pc->traits[i] > 0) ? 1 : 0);
		}
	for (i = 0; i < 5; i++) {
		cd_set_led(1013,36 + i,(store_pc->traits[10 + i] > 0) ? 1 : 0);
		}

	store = store_pc->get_tnl();
	cdsin(1013,18,store);
}

void pick_race_abil_event_filter(short item_hit)
{
	Str255 abil_str;
	cPlayer *pc;

	pc = store_pc;
			switch (item_hit) {
				case 3:
					toast_dialog();
					break;
				case 4: case 5: case 6:
					if (store_trait_mode == 0)
						pc->race = (eRace) (item_hit - 4);
					display_traits_graphics();
					get_str(abil_str,5,12 + item_hit);
					csit(1013,19,(char *) abil_str);
					break;
				case 36: case 37: case 38: case 39: case 40:
					if (store_trait_mode != 1)
						pc->traits[item_hit - 36 + 10] = (pc->traits[item_hit - 36 + 10] == true) ? false : true;
					display_traits_graphics();
					get_str(abil_str,5,item_hit - 36 + 11);				
					csit(1013,19,(char *) abil_str);
					break;
				default:
					if (item_hit >= 100)
						return;
					if (store_trait_mode != 1)
						pc->traits[item_hit - 7] = (pc->traits[item_hit - 7] == true) ? false : true;
					display_traits_graphics();
					get_str(abil_str,5,item_hit - 6);				
					csit(1013,19,(char *) abil_str);
					break;
				}	

}

void pick_race_abil(cPlayer *pc,short mode,short parent_num)
//mode; // 0 - edit  1 - just display  2 - can't change race
{
	char *start_str1 = "Click on button by name for description.";
	char *start_str2 = "Click on advantage button to add/remove.";
	short item_hit;
	
	store_trait_mode = mode;
	store_pc = pc;
	make_cursor_sword();

	cd_create_dialog_parent_num(1013,parent_num);

	display_traits_graphics();
	if (mode == 1)
		csit(1013,19,start_str1);
		else csit(1013,19,start_str2);
	
	item_hit = cd_run_dialog();
	cd_kill_dialog(1013);
	untoast_dialog();
}

void display_pc_info()
{
	short i,store;
	Str255 str;
	short pc;
	char to_draw[60];
	
	short weap1 = 24,weap2 = 24,hit_adj = 0, dam_adj = 0,skill_item;

	pc = store_pc_num;
	
	store = pc_carry_weight(pc);
	i = amount_pc_can_carry(pc);
	sprintf ((char *) to_draw, "%s is carrying %d stones out of %d.",univ.party[pc].name.c_str(),store,i);
	csit(1019,69,(char *) to_draw);

	sprintf((char *) str,"%d out of %d.",
			univ.party[pc].cur_health,univ.party[pc].max_health);
	csit(1019,65,(char *) str);
	sprintf((char *) str,"%d out of %d.",
			univ.party[pc].cur_sp,univ.party[pc].max_sp);
	csit(1019,67,(char *) str);

	for (i = 0; i < 19; i++) {
		cdsin(1019,18 + i * 2,univ.party[pc].skills[i]);
		}
	store = total_encumberance(pc);
	cdsin(1019,62,store);
	csit(1019,9,univ.party[pc].name.c_str());
	cdsin(1019,11,univ.party[pc].level);
	cdsin(1019,13,univ.party[pc].experience);
	cdsin(1019,71,univ.party[pc].skill_pts);
	store = univ.party[pc].level * univ.party[pc].get_tnl();
	cdsin(1019,15,store);
	csp(1019,7,800 + univ.party[pc].which_graphic,PICT_PC);

	// Fight bonuses
	for (i = 0; i < 24; i++)
		if (((univ.party[pc].items[i].variety == 1) || (univ.party[pc].items[i].variety == 2)) &&
			(univ.party[pc].equip[i] == true)) {
					if (weap1 == 24)
						weap1 = i;
						else weap2 = i;
					}
				
	hit_adj = stat_adj(pc,1) * 5 - (total_encumberance(pc)) * 5 
		+ 5 * minmax(-8,8,univ.party[pc].status[1]);
	if ((univ.party[pc].traits[2] == false) && (weap2 < 24))
		hit_adj -= 25;

	dam_adj = stat_adj(pc,0) + minmax(-8,8,univ.party[pc].status[1]);
	if ((skill_item = text_pc_has_abil_equip(pc,37)) < 24) {
		hit_adj += 5 * (univ.party[pc].items[skill_item].item_level / 2 + 1);
		dam_adj += univ.party[pc].items[skill_item].item_level / 2;
		}
	if ((skill_item = text_pc_has_abil_equip(pc,43)) < 24) {
		dam_adj += univ.party[pc].items[skill_item].item_level;
		hit_adj += univ.party[pc].items[skill_item].item_level * 2;
		}

		
	csit(1019,56,"No weapon.");	
	csit(1019,57,"");	
	csit(1019,59,"No weapon.");	
	csit(1019,60,"");	
	if (weap1 < 24) {
		if (!univ.party[pc].items[weap1].is_ident())
			csit(1019,56,"Not identified.");
			else {
				if (hit_adj + 5 * univ.party[pc].items[weap1].bonus < 0)
					sprintf(to_draw,"Penalty to hit: %%%d",hit_adj + 5 * univ.party[pc].items[weap1].bonus);
					else sprintf(to_draw,"Bonus to hit: +%%%d",hit_adj + 5 * univ.party[pc].items[weap1].bonus);
				csit(1019,56,to_draw);
				sprintf(to_draw,"Damage: (1-%d) + %d",univ.party[pc].items[weap1].item_level
					,dam_adj + univ.party[pc].items[weap1].bonus);
				csit(1019,57,to_draw);

				}
			}
	if (weap2 < 24) {
		if (!univ.party[pc].items[weap2].is_ident())
			csit(1019,59,"Not identified.");
			else {
				if (hit_adj + 5 * univ.party[pc].items[weap2].bonus < 0)
					sprintf(to_draw,"Penalty to hit: %%%d",hit_adj + 5 * univ.party[pc].items[weap2].bonus);
					else sprintf(to_draw,"Bonus to hit: +%%%d",hit_adj + 5 * univ.party[pc].items[weap2].bonus);
				csit(1019,59,to_draw);
				sprintf(to_draw,"Damage: (1-%d) + %d",univ.party[pc].items[weap2].item_level
					,dam_adj + univ.party[pc].items[weap2].bonus);
				csit(1019,60,to_draw);

				}
			}

}

void give_pc_info_event_filter(short item_hit)
{
	short pc;

	pc = store_pc_num;
	switch (item_hit) {
		case 1: 
			toast_dialog();
			break;
		case 2: 
			do				
				store_pc_num = (store_pc_num == 0) ? 5 : store_pc_num - 1;
				while (univ.party[store_pc_num].main_status != 1);
			display_pc_info();
			break;
		case 3: 
			do 
				store_pc_num = (store_pc_num + 1) % 6;
				while (univ.party[store_pc_num].main_status != 1);
			display_pc_info();
			break;
		case 4:
			display_pc(pc,0,1019);
			break;
		case 5:
			display_pc(pc,1,1019);
			break;
		case 6:
			pick_race_abil(&univ.party[pc],1,1019);
			break;
		case 68:
			display_alchemy();
			break;
		}	

}

void give_pc_info(short pc_num)
{
	short item_hit ,i;
	Str255 str;
	
	store_pc_num = pc_num;
	make_cursor_sword();

	cd_create_dialog_parent_num(1019,0);

	for (i = 0; i < 19; i++) {
		get_str(str,9,1 + i * 2);
		csit(1019,17 + i * 2,(char *) str);
		}
	display_pc_info(); 
	
	item_hit = cd_run_dialog();	
	cd_kill_dialog(1019);
}

void adventure_notes_event_filter (short item_hit)
{
	unsigned short i;
	Str255 place_str;
	
			switch (item_hit) {
				case 1: 
					toast_dialog();
					break;
				
				case 7: case 8:
					if (item_hit == 7) {
						if (store_page_on == 0)
							store_page_on = (store_num_i - 1) / 3;
							else store_page_on--;
						}
						else {
							if (store_page_on == (store_num_i - 1) / 3)
								store_page_on = 0;
								else store_page_on++;
							}					
					
					break;
				case 10: case 9: case 11:
					short which_to_delete = (store_page_on * 3) + item_hit - 9;
					cParty::encIter iter = univ.party.special_notes.begin();
					iter += which_to_delete;
					univ.party.special_notes.erase(iter);
					//univ.party.special_notes_str[(store_page_on * 3) + item_hit - 9][0] = -1;
					break;
				}
	for (i = 0; i < 3; i++) {
		if (univ.party.special_notes.size() > i) {
			switch (univ.party.special_notes[i].str_num / 1000) {
				case 0:
					strcpy((char *) place_str,scenario.scen_strs(univ.party.special_notes[i].str_num % 1000)); break;
				case 1:
					 load_outdoor_str(loc(univ.party.special_notes[i].where % scenario.out_width,
					 	univ.party.special_notes[i].where / scenario.out_width),univ.party.special_notes[i].str_num % 1000,(char *)place_str);
					break;
				case 2:
					load_town_str(univ.party.special_notes[i].where,univ.party.special_notes[i].str_num,(char *)place_str); break;
				}

			get_str(place_str,univ.party.special_notes[i].str_num,univ.party.special_notes[i].where);
			csit(961,3 + i,(char *) place_str);
			cd_activate_item(961,9 + i,1);
			}
			else cd_activate_item(961,9 + i,0);
		}
	for (i = store_page_on * 3; i < (store_page_on * 3) + 3; i++) {
		if (univ.party.special_notes.size() > i) {
			switch (univ.party.special_notes[i].str_num / 1000) {
				case 0:
					strcpy((char *) place_str,scenario.scen_strs(univ.party.special_notes[i].str_num % 1000)); break;
				case 1:
					 load_outdoor_str(loc(univ.party.special_notes[i].where % scenario.out_width,
					 	univ.party.special_notes[i].where / scenario.out_width), univ.party.special_notes[i].str_num % 1000,(char *)place_str);
					break;
				case 2:
					load_town_str(univ.party.special_notes[i].where,univ.party.special_notes[i].str_num % 1000,(char *)place_str); break;
				}


			csit(961,3 + (i - store_page_on * 3),(char *) place_str);
			cd_activate_item(961,9 + (i - store_page_on * 3),1);
			}
			else {
				csit(961,3 + (i - store_page_on * 3),"");
				cd_activate_item(961,9 + (i - store_page_on * 3),0);
				}
		}
}

void adventure_notes()
{

	unsigned short i;
	short item_hit;
	Str255 place_str;
	
	store_num_i = 0;
	//for (i = 0; i < 140; i++)
		//if (univ.party.special_notes_str[i][0] > 0)
			store_num_i = univ.party.special_notes.size(); //i + 1;
	store_page_on = 0;
	if (store_num_i == 0) {
		ASB("Nothing in your journal.");
		print_buf();
		return;
		}
	
	make_cursor_sword();

	cd_create_dialog_parent_num(961,0);

	for (i = 0; i < 3; i++) {
		if (univ.party.special_notes.size() > i) {
			switch (univ.party.special_notes[i].str_num / 1000) {
				case 0:
					strcpy((char *) place_str,scenario.scen_strs(univ.party.special_notes[i].str_num % 1000)); break;
				case 1:
					 load_outdoor_str(loc(univ.party.special_notes[i].where % scenario.out_width,
					 	univ.party.special_notes[i].where / scenario.out_width), univ.party.special_notes[i].str_num % 1000,(char *)place_str);
					break;
				case 2:
					load_town_str(univ.party.special_notes[i].where,univ.party.special_notes[i].str_num % 1000,(char *)place_str); break;
				}

			csit(961,3 + i,(char *) place_str);
			cd_activate_item(961,9 + i,1);
			}
			else cd_activate_item(961,9 + i,0);
		}
	if (store_num_i <= 3) {
		cd_activate_item(961,7,0);
		cd_activate_item(961,8,0);
	}
	
	item_hit = cd_run_dialog();
	cd_kill_dialog(961);

}

void put_talk()
{
	short personality;
	Str255 place_str;
	
	csit(960,3,"");
	csit(960,5,"");
	csit(960,7,"");
	csit(960,9,"");
	if ((personality = univ.party.talk_save[store_page_on].personality) >= 0) {
		if (personality / 10 != univ.town.cur_talk_loaded){
			if(personality / 10 == univ.town.num) univ.town.cur_talk = &univ.town->talking;
			else load_town(personality / 10,*univ.town.cur_talk);
			univ.town.cur_talk_loaded = personality / 10;
		}

		load_town_str(univ.party.talk_save[store_page_on].town_num,0,(char *) place_str);
		csit(960,9,(char *) place_str);
		
		//get_str(place_str,120 + ((personality - 1) / 10),((personality - 1) % 10) + 1);
		csit(960,7,univ.town.cur_talk->talk_strs[personality % 10]);

		if (univ.party.talk_save[store_page_on].str_num1 >= 1000) {
			if (univ.party.talk_save[store_page_on].str_num1 >= 3000) 
				csit(960,3,scenario.scen_strs(univ.party.talk_save[store_page_on].str_num1 - 3000));
				else {
					load_town_str(univ.party.talk_save[store_page_on].town_num,
						univ.party.talk_save[store_page_on].str_num1 - 2000 ,(char *) place_str);
					csit(960,3,(char *) place_str);
					}
			}
			else if (univ.party.talk_save[store_page_on].str_num1 > 0)
				csit(960,3,univ.town.cur_talk->talk_strs[univ.party.talk_save[store_page_on].str_num1]);

		if (univ.party.talk_save[store_page_on].str_num2 >= 1000) {
			if (univ.party.talk_save[store_page_on].str_num2 >= 3000) 
				csit(960,5,scenario.scen_strs(univ.party.talk_save[store_page_on].str_num2 - 3000 + 160));
				else {
					load_town_str(univ.party.talk_save[store_page_on].town_num,
						univ.party.talk_save[store_page_on].str_num2 - 2000/* + 20*/,(char *) place_str);
					csit(960,5,(char *) place_str);
					}
			}
			else if (univ.party.talk_save[store_page_on].str_num2 > 0)
				csit(960,5,univ.town.cur_talk->talk_strs[univ.party.talk_save[store_page_on].str_num2]);
	}
}

void talk_notes_event_filter (short item_hit)
{
			switch (item_hit) {
				case 1: 
					toast_dialog();
					break;
				
				case 10: case 11:
					if (item_hit == 10) {
						if (store_page_on == 0)
							store_page_on = store_num_i - 1;
							else store_page_on--;
						}
						else {
							if (store_page_on == store_num_i - 1)
								store_page_on = 0;
								else store_page_on++;
							}					
					
					break;
				case 12:
					univ.party.talk_save[store_page_on].personality = -1;
					break;
				}
	put_talk();
}

void talk_notes()
{

	short i,item_hit;
	
	store_num_i = 0;
	for (i = 0; i < 120; i++)
		if (univ.party.talk_save[i].personality != -1)
			store_num_i = i + 1;
	store_page_on = 0;
	if (store_num_i == 0) {
		ASB("Nothing in your talk journal.");
		print_buf();
		return;
		}
	
	make_cursor_sword();

	cd_create_dialog_parent_num(960,0);

	put_talk();
	if (store_num_i == 1) {
		cd_activate_item(960,10,0);
		cd_activate_item(960,11,0);
	}
	
	item_hit = cd_run_dialog();
	cd_kill_dialog(960);

}

void journal_event_filter (short item_hit)
{
	short i;
	Str255 place_str;
	
			switch (item_hit) {
				case 1: 
					toast_dialog();
					break;
				
				case 7: case 8:
					if (item_hit == 7) {
						if (store_page_on == 0)
							store_page_on = (store_num_i - 1) / 3;
							else store_page_on--;
						}
						else {
							if (store_page_on == (store_num_i - 1) / 3)
								store_page_on = 0;
								else store_page_on++;
							}					
					
					break;

				}
	for (i = 0; i < 3; i++) {
		if ((long)univ.party.journal.size() > i + (store_page_on * 3)) {
			////get_str(place_str,17,univ.party.journal_str[i + (store_page_on * 3)]);
			csit(962,3 + i,scenario.scen_strs(univ.party.journal[i].str_num + 10));
			sprintf((char *)place_str,"Day: %d",univ.party.journal[i + (store_page_on * 3)].day);
			csit(962,9 + i,(char *)place_str);
		}
		else{
			csit(962,3 + i,"");csit(962,9 + i,"");
		}
	}

}

void journal()
{

	unsigned short i;
	short item_hit;
	Str255 place_str;
	
	store_num_i = 0;
	//for (i = 0; i < 120; i++)
		//if (univ.party.journal[i].str_num > 0)
			store_num_i = univ.party.journal.size();//i + 1;
	store_page_on = 0;
	
	make_cursor_sword();

	cd_create_dialog_parent_num(962,0);

	for (i = 0; i < 3; i++) {
		if (univ.party.journal.size() > i) {
			////get_str(place_str,17,univ.party.journal_str[i]);
			csit(962,3 + i,scenario.scen_strs(univ.party.journal[i].str_num + 10));
			sprintf((char *)place_str,"Day: %d",univ.party.journal[i].day);
			csit(962,9 + i,(char *)place_str);
			}
		}
	if (store_num_i <= 3) {
		cd_activate_item(962,7,0);
		cd_activate_item(962,8,0);
	}
	
	item_hit = cd_run_dialog();	
	cd_kill_dialog(962);

}
void add_to_journal(short event)
{
	if(univ.party.add_to_journal(event, calc_day()))
	   ASB("Something was added to your journal.");
}


// Only call these when main window is in foreground!
void display_enc_string(short val1,short val2,short sound)
{
//	display_strings(val1,val2,0,0,"",sound,708,0);	
}
void display_2_enc_string(short val1,short val2,short val3,short val4,short sound)
{
//	display_strings(val1,val2,val3,val4,"",sound,708,0);	
}

// Call call this anywhere, but don't forget parent!!!
void give_help(short help1,short help2,short parent_num)
{
	bool help_forced = false;
	Str255 str1,str2;
	
	if (help1 >= 200) {
		help_forced = true;
		help1 -= 200;
		}
	if ((PSD[SDF_NO_INSTANT_HELP] > 0) && (help_forced == false))
		return;
	if (univ.party.help_received[help1] > 0)
		return;
	//if (help1 >= 20)
		univ.party.help_received[help1] = 1;
	get_str(str1,10,help1);
	if (help2 > 0)
		get_str(str2,10,help2);
	if (help2 == 0)
		display_strings((char *)str1, "",-1,-1,-1,-1,"Instant Help",57,24,PICT_DLG, parent_num);
	else display_strings((char *)str1,(char *)str2,-1,-1,-1,-1,"Instant Help",57,24,PICT_DLG, parent_num);
}

void put_spec_item_info (short which_i)
{
	
	display_strings(scenario.scen_strs(60 + 1 + which_i * 2),"",
	-1,-1,-1,-1,
	scenario.scen_strs(60 + which_i * 2),57,scenario.intro_pic,PICT_SCEN,0);
	//get_str(item_name,6,1 + which_i * 2);
	//display_strings(6,2 + which_i * 2,0,0,
	//(char *)item_name,-1,702,0);
}

void record_display_strings(){
	bool had1 = false, had2 = false;
	unsigned int i;
	play_sound(0);
	for (i = 0; i < univ.party.special_notes.size(); i++)
		if ((store_str_label_1 == univ.party.special_notes[i].str_num) &&
			(store_str_label_1b == univ.party.special_notes[i].where))
			had1 = true;
	if (!had1) {
	   give_help(58,0,store_which_string_dlog);
	   univ.party.record(store_str_label_1, store_str_label_1b);
	}
	for (i = 0; i < univ.party.special_notes.size(); i++)
		if ((store_str_label_2 == univ.party.special_notes[i].str_num) &&
			(store_str_label_2b == univ.party.special_notes[i].where))
			had2 = true;
	if (!had2) {
	   univ.party.record(store_str_label_2,store_str_label_2b);
	}
}