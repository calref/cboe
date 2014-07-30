#include <windows.h>
#include <cstring>
#include "global.h"
#include "boe.dlgutil.h"
#include "boe.text.h"
#include "boe.town.h"
#include "boe.itemdata.h"
#include "boe.locutils.h"
#include "boe.fields.h"
#include "boe.party.h"
#include "boe.specials.h"
#include "boe.fileio.h"
#include "boe.graphics.h"
#include "boe.items.h"
#include "tools/soundtool.h"
#include "tools/mathutil.h"
#include <cstdio>
#include "tools/dlogtool.h"
#include "boe.newgraph.h"
#include "boe.infodlg.h"
#include "boe.graphutil.h"

#include "globvar.h"

#define	NUM_HINTS	30

extern BOOL play_startup;

/*
shop_type:
0 - weapon shop
1 - armor shop
2 - misc shop
3 - healer
4 - food
5-9 - magic shop
10 - mage spells
11 - priest spells
12 alchemy
*/
void start_shop_mode(short shop_type,short shop_min,short shop_max,short cost_adj,char *store_name)
{
	RECT area_rect;

    if(PSD[SDF_ASK_ABOUT_TEXT_BOX] == 1)
    	ShowWindow(talk_edit_box, SW_HIDE);

	if (shop_max < shop_min)
		shop_max = shop_min; ////
	store_cost_mult = cost_adj;
	if (store_cost_mult > 6)
		store_cost_mult = 6;
	store_shop_type = shop_type;
	store_shop_min = shop_min;
	store_shop_max = shop_max;

	area_rect = talk_area_rect;
	strcpy((char *) store_store_name,store_name);

	OffsetRect(&area_rect, -1 * area_rect.left,-1 * area_rect.top);
	if (talk_gworld == NULL)
		 talk_gworld = CreateCompatibleBitmap(main_dc,area_rect.right,area_rect.bottom);

	store_pre_shop_mode = overall_mode;
	overall_mode = MODE_SHOPPING;
	stat_screen_mode = 1;
	create_clip_region();

	set_up_shop_array();
	put_background();

	draw_shop_graphics(0,area_rect);

	put_item_screen(stat_window,0);
	give_help(26,27,0);
}

void end_shop_mode()
{
	RECT dummy_rect = {0,0,0,0};

    if(PSD[SDF_ASK_ABOUT_TEXT_BOX] == 1)
    	ShowWindow(talk_edit_box, SW_SHOW);

	ShowScrollBar(shop_sbar,SB_CTL,false);
	if (store_pre_shop_mode == 20) {
		sprintf(old_str1,"You conclude your business.");
		sprintf(old_str2,"");
		sprintf(one_back1,"You conclude your business.");
		sprintf(one_back2,"");

		strnum1 = strnum2 = oldstrnum1 = oldstrnum2 = 0;
		place_talk_str(old_str1,"",0,dummy_rect);
		}
		else {
			DeleteObject(talk_gworld);
			talk_gworld = NULL;
			}

	overall_mode = store_pre_shop_mode;
	create_clip_region();
	if (overall_mode == MODE_TALK_TOWN)
		overall_mode = MODE_TOWN;
	if (overall_mode == MODE_TOWN) {
		center = c_town.p_loc;
		update_explored(center);
		}
	stat_screen_mode = 0;
	put_item_screen(stat_window,0);
	put_pc_screen();
	refresh_screen(0);
}

void handle_shop_event(POINT p)
{
	short i,store_what_picked;

	p.x -= 5;
	p.y -= 5;

	if (PtInRect(&talk_help_rect,p)) {
		click_shop_rect(talk_help_rect);
		party.help_received[26] = 0;
		give_help(226,27,0);
		return;
		}
	if (PtInRect(&shop_done_rect, p)) {
		click_shop_rect(shop_done_rect);
		end_shop_mode();
		return;
		}

	for (i = 0; i < 8; i++) {
		store_what_picked = i + GetScrollPos(shop_sbar,SB_CTL);
		if ((PtInRect(&shopping_rects[i][1],p)) && (store_shop_items[store_what_picked] >= 0)) {
			click_shop_rect(shopping_rects[i][1]);
			handle_sale(store_shop_items[store_what_picked],store_shop_costs[store_what_picked]);
			}
		if ((PtInRect(&shopping_rects[i][6],p)) && (store_shop_items[store_what_picked] >= 0)
			&& (store_shop_type != SHOP_HEALER) && (store_shop_type != SHOP_FOOD)){
			click_shop_rect(shopping_rects[i][6]);
			handle_info_request(store_shop_items[store_what_picked]);
			}
		}
}

void handle_sale(short what_chosen,short cost)
{
	item_record_type base_item;
	short what_magic_shop,what_magic_shop_item,i;
	RECT dummy_rect = {0,0,0,0};

	switch (what_chosen / 100) {
		case 0: case 1: case 2: case 3: case 4:
			base_item = get_stored_item(what_chosen);
			base_item.item_properties = base_item.item_properties | 1;
			//cost = (base_item.charges == 0) ? base_item.value : base_item.value * base_item.charges;
			switch (adven[current_pc].okToBuy(cost,base_item)) {
				case 1: play_sound(-38); adven[current_pc].giveToPC(base_item,true); break;
				case 2: ASB("Can't carry any more items."); break;
				case 3: ASB("Not enough cash."); break;
				case 4: ASB("Item is too heavy."); break;
				case 5: ASB("You own too many of this."); break;
				}
			break;
		case 5:
			base_item = store_alchemy(what_chosen - 500);
			if (party.alchemy[base_item.item_level] == true)
				ASB("You already know that recipe.");
				else if (party.takeGold(cost, false) == false)
					ASB("Not enough gold.");
					else {
						play_sound(62);
						ASB("You buy an alchemical recipe.");
						party.alchemy[base_item.item_level] = true;
						}
			break;
		case 7:
			what_chosen -= 700;
			if (party.takeGold(cost, false) == false)
				ASB("Not enough gold.");
				else {
					ASB("You pay the healer.");
					play_sound(68);
					switch (what_chosen) {
						case 0:
							adven[current_pc].cur_health = adven[current_pc].max_health;
							break;
						case 1:
							adven[current_pc].status[STATUS_POISON] = 0;
							break;
						case 2:
							adven[current_pc].status[STATUS_DISEASE] = 0; break;
						case 3:
							adven[current_pc].status[STATUS_PARALYZED] = 0; break;
						case 4:
							for (i = 0; i < 24; i++)
								if ((adven[current_pc].equip[i] == true) &&
									(adven[current_pc].items[i].isCursed()))
										adven[current_pc].items[i].item_properties =
											adven[current_pc].items[i].item_properties & 239;
  							break;
						case 5: case 6: case 7:
							adven[current_pc].main_status = MAIN_STATUS_ALIVE; break;
						case 8:
							adven[current_pc].status[STATUS_DUMB] = 0; break;
						}
					}
			break;
		case 8:
			base_item = store_mage_spells(what_chosen - 800 - 30);
			if ((base_item.item_level < 0) || (base_item.item_level > 61)) {
				MessageBeep(MB_OK); ASB("Error 102: Report this!"); break;}
			if (adven[current_pc].mage_spells[base_item.item_level] == true)
				ASB("You already have this spell.");
				else if (party.takeGold(cost, false) == false)
					ASB("Not enough gold.");
					else {
						play_sound(62);
						ASB("You buy a spell.");
						adven[current_pc].mage_spells[base_item.item_level] = true;
						give_help(41,0,0);
						}
			break;
		case 9:
			base_item = store_priest_spells(what_chosen - 900 - 30);
			if ((base_item.item_level < 0) || (base_item.item_level > 61)) {
				MessageBeep(MB_OK); ASB("Error 101: Report this!"); break;}
			if (adven[current_pc].priest_spells[base_item.item_level] == true)
				ASB("You already have this spell.");
				else if (party.takeGold(cost, false) == false)
					ASB("Not enough gold.");
					else {
						play_sound(62);
						ASB("You buy a spell.");
						adven[current_pc].priest_spells[base_item.item_level] = true;
						give_help(41,0,0);
						}
			break;
		default:
			what_magic_shop = (what_chosen / 1000) - 1;
			what_magic_shop_item = what_chosen % 1000;
			base_item = party.magic_store_items[what_magic_shop][what_magic_shop_item];
			base_item.item_properties = base_item.item_properties | 1;
			switch (adven[current_pc].okToBuy(cost,base_item)) {
				case 1: play_sound(-38); adven[current_pc].giveToPC(base_item,true);
					party.magic_store_items[what_magic_shop][what_magic_shop_item].variety = ITEM_TYPE_NO_ITEM;
					break;
				case 2: ASB("Can't carry any more items."); break;
				case 3: ASB("Not enough cash."); break;
				case 4: ASB("Item is too heavy."); break;
				}
			break;
		}
	set_up_shop_array();

	if (overall_mode != MODE_SHOPPING) {
		MessageBeep(MB_OK);
		ASB("Shop error 1. Report This!");
		}
	draw_shop_graphics(0,dummy_rect);
	print_buf();
	put_pc_screen();
	put_item_screen(stat_window,0);
}


void handle_info_request(short what_chosen)
{
	item_record_type base_item;
	short what_magic_shop,what_magic_shop_item;

	switch (what_chosen / 100) {
		case 0: case 1: case 2: case 3: case 4:
			base_item = get_stored_item(what_chosen);
			base_item.item_properties = base_item.item_properties | 1;
			display_pc_item(6,0, base_item,0);
			break;
		case 5:
			display_help(0,0);
			break;
		case 8:
			base_item = store_mage_spells(what_chosen - 800 - 30);
			display_spells(0,base_item.item_level,0);
			break;
		case 9:
			base_item = store_priest_spells(what_chosen - 900 - 30);
			display_spells(1,base_item.item_level,0);
			break;
		default:
			what_magic_shop = (what_chosen / 1000) - 1;
			what_magic_shop_item = what_chosen % 1000;
			base_item = party.magic_store_items[what_magic_shop][what_magic_shop_item];
			base_item.item_properties = base_item.item_properties | 1;
			display_pc_item(6,0, base_item,0);
			break;
	}
}

void set_up_shop_array()
{
	short i,shop_pos = 0;
	Boolean cursed_item = false;
	item_record_type store_i;
	long store_l;

	for (i = 0; i < 30; i++)
		store_shop_items[i] = -1;
	switch (store_shop_type) {
		case SHOP_WEAPON_SHOP: case SHOP_ARMOR_SHOP: case SHOP_MISC_SHOP:
			for (i = store_shop_min; i < store_shop_max + 1; i++) {
				store_shop_items[shop_pos] = i;
				store_i = get_stored_item(store_shop_items[shop_pos]);
				store_shop_costs[shop_pos] = (store_i.charges == 0) ?
				  store_i.value : store_i.value * store_i.charges;
				shop_pos++;
				}
			break;
		case SHOP_HEALER:
			if (adven[current_pc].cur_health < adven[current_pc].max_health) {
				store_shop_items[shop_pos] = 700;
				store_shop_costs[shop_pos] = heal_costs[0];
				shop_pos++;
				}
			if (adven[current_pc].status[STATUS_POISON] > 0) {
				store_shop_items[shop_pos] = 701;
				store_shop_costs[shop_pos] = heal_costs[1];
				shop_pos++;
				}
			if (adven[current_pc].status[STATUS_DISEASE] > 0) {
				store_shop_items[shop_pos] = 702;
				store_shop_costs[shop_pos] = heal_costs[2];
				shop_pos++;
				}
			if (adven[current_pc].status[STATUS_PARALYZED] > 0) {
				store_shop_items[shop_pos] = 703;
				store_shop_costs[shop_pos] = heal_costs[3];
				shop_pos++;
				}
			if (adven[current_pc].status[STATUS_DUMB] > 0) {
				store_shop_items[shop_pos] = 708;
				store_shop_costs[shop_pos] = heal_costs[8];
				shop_pos++;
				}
			for (i = 0; i < 24; i++)
				if ((adven[current_pc].equip[i] == true) && (adven[current_pc].items[i].isCursed()))
					cursed_item = true;
  			if (cursed_item) {
				store_shop_items[shop_pos] = 704;
				store_shop_costs[shop_pos] = heal_costs[4];
				shop_pos++;
				}
			if (adven[current_pc].main_status == MAIN_STATUS_STONE) {
				store_shop_items[shop_pos] = 705;
				store_shop_costs[shop_pos] = heal_costs[5];
				shop_pos++;
				}
			if (adven[current_pc].main_status == MAIN_STATUS_DEAD){
				store_shop_items[shop_pos] = 706;
				store_shop_costs[shop_pos] = heal_costs[6];
				shop_pos++;
				}
			if  (adven[current_pc].main_status == MAIN_STATUS_DUST){
				store_shop_items[shop_pos] = 707;
				store_shop_costs[shop_pos] = heal_costs[7];
				shop_pos++;
				}
			break;
		case SHOP_FOOD:
			break;
		case SHOP_MAGIC_SHOP_1: case SHOP_MAGIC_SHOP_2: case SHOP_MAGIC_SHOP_3: case SHOP_MAGIC_SHOP_4: case SHOP_MAGIC_SHOP_5:
			for (i = 0; i < 10; i++)
				if (party.magic_store_items[store_shop_type - 5][i].variety != ITEM_TYPE_NO_ITEM) {
					store_shop_items[shop_pos] = (store_shop_type - 4) * 1000 + i;
					store_i = party.magic_store_items[store_shop_type - 5][i];
					store_shop_costs[shop_pos] = (store_i.charges == 0) ?
					  store_i.value : store_i.value * store_i.charges;
					shop_pos++;
					}
			break;
		case SHOP_MAGE_SPELLS:
			for (i = store_shop_min; i < store_shop_max + 1; i++)
				if (i == minmax(0,31,(int)i)) {
				store_i = store_mage_spells(i);
				store_shop_costs[shop_pos] = store_i.value;
				store_shop_items[shop_pos] = 800 + i + 30;
				shop_pos++;
				}
			break;
		case SHOP_PRIEST_SPELLS:
			for (i = store_shop_min; i < store_shop_max + 1; i++)
				if (i == minmax(0,31,(int)i)) {
				store_i = store_priest_spells(i);
				store_shop_costs[shop_pos] = store_i.value;
				store_shop_items[shop_pos] = 900 + i + 30;
				shop_pos++;
				}
			break;
		case SHOP_ALCHEMY:
			for (i = store_shop_min; i < store_shop_max + 1; i++)
				if (i == minmax(0,19,(int)i)) {
				store_i = store_alchemy(i);
				store_shop_costs[shop_pos] = store_i.value;
				store_shop_items[shop_pos] = 500 + i;
				shop_pos++;
				}
			break;
		}
	for (i = 0; i < 30; i++)
		if (store_shop_items[i] >= 0) {
			store_l = store_shop_costs[i];
			store_l = (store_l * cost_mult[store_cost_mult]) / 10;
			store_shop_costs[i] = (short) store_l;
			}
  	i = max(0,shop_pos - 8);
    lpsi.nMax = i;
    lpsi.fMask = SIF_RANGE;
    SetScrollInfo(shop_sbar,SB_CTL,&lpsi,false);
	//SetScrollRange(shop_sbar,SB_CTL,0,i,true);

}

void start_talk_mode(short m_num,short personality,unsigned char monst_type,short store_face_pic)////
{
	RECT area_rect, talk_edit_box_rect;
	char place_string1[256] = "";
	char place_string2[256] = "";

	store_personality = personality;

	store_monst_type = monst_type;
	store_m_num = m_num;
	store_talk_face_pic = store_face_pic; ////
	area_rect = talk_area_rect;
	OffsetRect(&area_rect, -1 * area_rect.left,-1 * area_rect.top);
   	talk_gworld = CreateCompatibleBitmap(main_dc,area_rect.right,area_rect.bottom);

    if(PSD[SDF_ASK_ABOUT_TEXT_BOX] == 1){
        talk_edit_box_rect = preset_words[8].word_rect;
        OffsetRect(&talk_edit_box_rect,ulx,uly+3);
    	talk_edit_box = CreateWindow("edit",NULL,WS_CHILD | WS_BORDER | WS_VISIBLE,
									talk_edit_box_rect.left+125,talk_edit_box_rect.top,
									140,
									20,
									mainPtr,(HMENU) 160,(HINSTANCE) store_hInstance,NULL);
        store_edit_parent =  mainPtr;
    	old_edit_proc = (WNDPROC) (GetWindowLongPtr(talk_edit_box,GWLP_WNDPROC));
    	SetWindowLongPtr(talk_edit_box,GWLP_WNDPROC,(LONG_PTR) edit_proc);
    	SetFocus(talk_edit_box);
        }

	// first make sure relevant talk strs are loaded in
	if (personality / 10 != cur_town_talk_loaded)
		load_town(personality / 10,1,0,NULL);

	// Dredge up critter's name
	sprintf((char *) title_string,"%s:",data_store3->talk_strs[personality % 10]);

	store_pre_talk_mode = overall_mode;
	overall_mode = MODE_TALKING;
	create_clip_region();
	talk_end_forced = false;
	stat_screen_mode = 1;

	// Bring up and place first strings.
	sprintf((char *) place_string1,"%s",data_store3->talk_strs[personality % 10 + 10]);
	strnum1 = personality % 10 + 10;
	strnum2 = 0;
	strcpy((char *) old_str1,(char *) place_string1);
	strcpy((char *) old_str2,(char *) place_string2);
	strcpy((char *) one_back1,(char *) place_string1);
	strcpy((char *) one_back2,(char *) place_string2);
	place_talk_str((char *) place_string1,(char *) place_string2,0,dummy_rect);

	put_item_screen(stat_window,0);
	give_help(5,6,0);

}

void end_talk_mode()
{

	DeleteObject(talk_gworld);
	talk_gworld = NULL;
    if((PSD [SDF_ASK_ABOUT_TEXT_BOX] == 1) && (talk_edit_box != NULL)){
        DestroyWindow(talk_edit_box);
	    talk_edit_box = NULL;
    }

    overall_mode = store_pre_talk_mode;

	create_clip_region();
	if (overall_mode == MODE_TALK_TOWN)
		overall_mode = MODE_TOWN;
	if (overall_mode == MODE_TOWN) {
		center = c_town.p_loc;
		update_explored(center);
		}

	stat_screen_mode = 0;

    put_item_screen(stat_window,0);
	put_pc_screen();
	redraw_screen(0);


}

void handle_talk_event(POINT p)
{
	short i,j,force_special = 0,get_pc,s1 = -1,s2 = -1,s3 = -1;
	char asked[4];
	char place_string1[256] = "";
	char place_string2[256] = "";

	short a,b,c,d,ttype,which_talk_entry = -1;

	p.x -= 5;
	p.y -= 5;

	if (PtInRect(&talk_help_rect,p)) {
    	if (play_sounds == true) play_sound(37);
		party.help_received[5] = 0;
		give_help(205,6,0);
		return;
		}

	for (i = 0; i < 9; i++)
		if ((PtInRect(&preset_words[i].word_rect,p)) && ((talk_end_forced == false) || (i == 6) || (i == 5))) {
			click_talk_rect((char *) old_str1,(char *) old_str2,preset_words[i].word_rect);
			switch (i) {
				case 0: case 1: case 2: case 7: case 8:
					force_special = i + 1;
					break;
				case 3: case 4:
					force_special = i + 1;
					break;
				case 5: // save
					if (strnum1 <= 0) {
						MessageBeep(MB_OK);
						return;
						}
					for (j = 0; j < 120; j++)
						if ((party.talk_save[j].personality == store_personality) &&
						  (party.talk_save[j].str1 == strnum1) &&
						  (party.talk_save[j].str2 == strnum2)) {
						  	ASB("This is already saved.");
						  	print_buf();
						  	return;
						  	}
					for (j = 0; j < 120; j++)
						if (party.talk_save[j].personality <= 0) {
							give_help(57,0,0);
							play_sound(0);
							party.talk_save[j].personality = store_personality;
							party.talk_save[j].town_num = (unsigned char) c_town.town_num;
							party.talk_save[j].str1 = strnum1;
							party.talk_save[j].str2 = strnum2;
							ASB("Noted in journal.");
							j = 200;
							}
					if (j < 200) {
						MessageBeep(MB_OK);
						ASB("No more room in talking journal.");
						}
					print_buf();
					return;
					break;
				case 6: // quit
					end_talk_mode();

                	if(talk_end_forced == 10){ // party is in inn
                        talk_end_forced = true; // safety set
                        for(j=0; j < 700; j++){
                            party.age++;
                            // Specials countdowns
                            if ((party.age % 500 == 0) && (get_ran(1,0,5) == 3) && (adven.hasAbil(ITEM_DISEASE_PARTY) == true)) {
                                j = 900;
                                adven.disease(2);
                             }
                             // Plants and magic shops
                             if (party.age % 4000 == 0) refresh_store_items();

                             timed_special_happened = special_increase_age(0);//don't delay the trigger of the special, if there's a special

                             if(timed_special_happened && PSD[SDF_COMPATIBILITY_SPECIALS_INTERRUPT_REST] == 1){
                                j = 900;
               			        add_string_to_buf("  Rest interrupted.");
               			        print_buf();
                            }
                         }
                      }
					return;
					break;
				default:
					for (j = 0; j < 4; j++)
						asked[j] = preset_words[i].word[j];
					break;
				}
			i = 100;
			}
	if (i < 100) {
		for (i = 0; i < 50; i++)
			if ((PtInRect(&store_words[i].word_rect,p)) && (talk_end_forced == false)) {
				click_talk_rect((char *) old_str1,(char *) old_str2,store_words[i].word_rect);
				for (j = 0; j < 4; j++)
					asked[j] = store_words[i].word[j];

				i = 100;
				}
		}
    if(strcmp(talk_edit_string,"") != 0){
        for(j = 0; j < 4; j++)
            asked[j] = talk_edit_string[j];
        talk_edit_string[0] = '\0';
        i =  100;
        }

	if (i == 50) // no event
		return;
	if (force_special == 9) {
		get_text_response(1017,place_string1,0);
		asked[0] = place_string1[0];
		asked[1] = place_string1[1];
		asked[2] = place_string1[2];
		asked[3] = place_string1[3];
		}

	if ((asked[0] == 'n') && (asked[1] == 'a') &&(asked[2] == 'm') &&(asked[3] == 'e')) {
		force_special = 2;
		}
	if ((asked[0] == 'l') && (asked[1] == 'o') &&(asked[2] == 'o') &&(asked[3] == 'k')) {
		force_special = 1;
		}
	if (((asked[0] == 'j') && (asked[1] == 'o') &&(asked[2] == 'b')) ||
		((asked[0] == 'w') && (asked[1] == 'o') &&(asked[2] == 'r')&&(asked[3] == 'k')) ) {
		force_special = 3;
		}
	if((asked[0] == 'b') && (asked[1] == 'u') && (asked[2] == 'y'))
	   force_special = 4;
	if((asked[0] == 'b') && (asked[1] == 'y') && (asked[2] == 'e')){
		end_talk_mode();
        return;
        }

	if (force_special > 0) {
		switch (force_special) {
			case 1: case 2: case 3:
				GetIndString(place_string1,120 + ((store_personality - 1) / 10),
				 ((store_personality - 1) % 10) * 3 + 10 + force_special);
				sprintf((char *) place_string1,"%s",data_store3->talk_strs[store_personality % 10 + 10 * force_special]);

				oldstrnum1 = strnum1; oldstrnum2 = strnum2;
				strnum1 =  store_personality % 10 + 10 * force_special;
				strnum2 = 0;
				strcpy((char *) one_back1,(char *) old_str1);
				strcpy((char *) one_back2,(char *) old_str2);
				strcpy((char *) old_str1,(char *) place_string1);
				strcpy((char *) old_str2,(char *) place_string2);
				place_talk_str((char *) place_string1,(char *) place_string2,0,dummy_rect);
				return;
				break;
			case 4: // buy button
				asked[0] = 'p';asked[1] = 'u';asked[2] = 'r';asked[3] = 'c';
				if (scan_for_response(asked) >= 0)
					break;
				asked[0] = 's';asked[1] = 'a';asked[2] = 'l';asked[3] = 'e';
				if (scan_for_response(asked) >= 0)
					break;
				asked[0] = 'h';asked[1] = 'e';asked[2] = 'a';asked[3] = 'l';
				if (scan_for_response(asked) >= 0)
					break;
				asked[0] = 'i';asked[1] = 'd';asked[2] = 'e';asked[3] = 'n';
				if (scan_for_response(asked) >= 0)
					break;
				asked[0] = 't';asked[1] = 'r';asked[2] = 'a';asked[3] = 'i';
				if (scan_for_response(asked) >= 0)
					break;
				break;
			case 5: // sell button
				asked[0] = 's';asked[1] = 'e';asked[2] = 'l';asked[3] = 'l';
				if (scan_for_response(asked) >= 0)
					break;
				break;
			case 8: // back 1
				strnum1 = oldstrnum1; strnum2 = oldstrnum2;
				strcpy((char *) place_string1,(char *) one_back1);
				strcpy((char *) place_string2,(char *) one_back2);
				strcpy((char *) one_back1,(char *) old_str1);
				strcpy((char *) one_back2,(char *) old_str2);
				strcpy((char *) old_str1,(char *) place_string1);
				strcpy((char *) old_str2,(char *) place_string2);
				place_talk_str((char *) place_string1,(char *) place_string2,0,dummy_rect);
				return;
				break;
			}
		}

	which_talk_entry = scan_for_response(asked);
	if ((which_talk_entry < 0) || (which_talk_entry > 59)) {
		strcpy((char *) one_back1,(char *) old_str1);
		strcpy((char *) one_back2,(char *) old_str2);
		sprintf((char *) old_str2,"");
		sprintf((char *) old_str1,"%s",data_store3->talk_strs[store_personality % 10 + 160]);
		if (strlen((char *) old_str1) < 2)
			sprintf((char *) old_str1,"You get no response.");
		place_talk_str((char *) old_str1,(char *) old_str2,0,dummy_rect);
		strnum1 = -1;
		return;
		}

	ttype = talking.talk_nodes[which_talk_entry].type;
	a = talking.talk_nodes[which_talk_entry].extras[0];
	b = talking.talk_nodes[which_talk_entry].extras[1];
	c = talking.talk_nodes[which_talk_entry].extras[2];
	d = talking.talk_nodes[which_talk_entry].extras[3];

	sprintf(place_string1,"%s",data_store3->talk_strs[40 + which_talk_entry * 2]);
	sprintf(place_string2,"%s",data_store3->talk_strs[40 + which_talk_entry * 2 + 1]);

	oldstrnum1 = strnum1; oldstrnum2 = strnum2;
	strnum1 =  40 + which_talk_entry * 2; strnum2 = 40 + which_talk_entry * 2 + 1;

	switch(ttype) {
		case TALK_REGULAR:
			break;
		case TALK_DEP_ON_SDF:
			if (PSD[a][b] > c) {
				strnum1 = strnum2;
				strcpy(place_string1, place_string2);
				}
			sprintf(place_string2,"");
			strnum2 = 0;
			break;
		case TALK_SET_SDF:
			PSD[a][b] = 1;
			break;
		case TALK_INN:
			if (party.gold < a) {
				strnum1 = strnum2;
				strcpy(place_string1, place_string2);
				}
				else {
					talk_end_forced = true;
					party.gold -= a;
					put_pc_screen();
                    if(PSD[SDF_COMPATIBILITY_CHECK_TIMERS_WHILE_RESTING] == 1){
                       talk_end_forced = 10;
                       adven.heal(30 * b);
	                   adven.restoreSP(25 * b);
      				   c_town.p_loc.x = c;
	          		   c_town.p_loc.y = d;
			           center = c_town.p_loc;
                       }
					else{
                    adven.heal(30 * b);
					adven.restoreSP(25 * b);
					party.age += 700;
					c_town.p_loc.x = c;
					c_town.p_loc.y = d;
					center = c_town.p_loc;
                    }
				}
			strnum2 = 0;
			sprintf(place_string2,"");
			break;
		case TALK_DEP_ON_TIME:
			if (day_reached((unsigned char) a,0) == true) {
				strnum1 = strnum2;
				strcpy(place_string1, place_string2);
				}
			sprintf(place_string2,"");
			strnum2 = 0;
			break;
		case TALK_DEP_ON_TIME_AND_EVENT:
			if (day_reached((unsigned char) a,(unsigned char) b) == true) {
				strnum1 = strnum2;
				strcpy(place_string1, place_string2);
				}
			sprintf(place_string2,"");
			strnum2 = 0;
			break;
		case TALK_DEP_ON_TOWN:
			if (c_town.town_num != a) {
				strnum1 = strnum2;
				strcpy(place_string1, place_string2);
				}
			sprintf(place_string2,"");
			strnum2 = 0;
			break;
		case TALK_BUY_ITEMS:
			c = minmax(1,30,(int)c);
			start_shop_mode(SHOP_MISC_SHOP,b,
				b + c - 1,a, place_string1);
			strnum1 = -1;
			return;
		case TALK_TRAINING:
			if ((get_pc = char_select_pc(1,0,"Train who?")) < 6) {
				strnum1 = -1;
				spend_xp(get_pc,1, 0);
				}
			sprintf(place_string1, "You conclude your training.");
            put_pc_screen();
			return;

		case TALK_BUY_MAGE: case TALK_BUY_PRIEST: case TALK_BUY_ALCHEMY:
			c = minmax(1,30,(int)c);
			start_shop_mode(ttype + 1,b,
				b + c - 1,a, place_string1);
			strnum1 = -1;
			return;
		case TALK_BUY_HEALING: //healer
			start_shop_mode(SHOP_HEALER,0,0,a, place_string1);
			strnum1 = -1;
			return;
			break;
		case TALK_SELL_WEAPONS: // sell weap
			strnum1 = -1;
			stat_screen_mode = 3;
			put_item_screen(stat_window,1);
			give_help(42,43,0);
			break;
		case TALK_SELL_ARMOR: // sell armor
			strnum1 = -1;
			stat_screen_mode = 4;
			put_item_screen(stat_window,1);
			give_help(42,43,0);
			break;
		case TALK_SELL_ITEMS: // sell misc
			strnum1 = -1;
			stat_screen_mode = 5;
			put_item_screen(stat_window,1);
			give_help(42,43,0);
			break;
		case TALK_IDENTIFY: case TALK_ENCHANT: // ident, enchant
            strnum1 = -1;
			stat_screen_mode = (ttype == 16) ? 2 : 6;
			shop_identify_cost = a;
			put_item_screen(stat_window,1);
			give_help(ttype - 16 + 44,0,0);
			break;
		case TALK_BUY_INFO:
			if (party.gold < a) {
				strnum1 = strnum2;
				strcpy(place_string1, place_string2);
				}
				else {
					party.gold -= a;
					put_pc_screen();

					}
			sprintf(place_string2,"");
			strnum2 = 0;
			break;
		case TALK_BUY_SDF:
			if ((sd_legit(b,c) == true) && (PSD[b][c] == d)) {
				sprintf(place_string1, "You've already learned that.");
				strnum1 = -1;
				}
			else if (party.gold < a) {
				strnum1 = strnum2;
				strcpy(place_string1, place_string2);
				}
				else {
					party.gold -= a;
					put_pc_screen();
					if (sd_legit(b,c) == true)
						PSD[b][c] = d;
						else give_error("Invalid Stuff Done flag called in conversation.","",0);
					}
			strnum2 = 0;
			sprintf(place_string2,"");
			break;
		case TALK_BUY_SHIP:
			if (party.gold < a) {
				strnum1 = strnum2;
				strnum2 = 0;
				strcpy(place_string1, place_string2);
				sprintf(place_string2,"");
				break;
				}
				else {
					for (i = b; i <= b + c; i++)
						if ((i >= 0) && (i < 30) && (party.boats[i].property == true)) {
							party.gold -= a;
							put_pc_screen();
							party.boats[i].property = false;
							sprintf(place_string2,"");
							strnum2 = 0;
							i = 1000;
							}
					if (i >= 1000)
						break;
					}
			sprintf(place_string1, "There are no boats left.");
			sprintf(place_string2,"");
			strnum1 = -1;
			strnum2 = -1;
			break;
		case TALK_BUY_HORSE:
			if (party.gold < a) {
				strnum1 = strnum2;
				strnum2 = 0;
				strcpy(place_string1, place_string2);
				sprintf(place_string2,"");
				break;
				}
				else {
					for (i = b; i <= b + c; i++)
						if ((i >= 0) && (i < 30) && (party.horses[i].property == true)) {
							party.gold -= a;
							put_pc_screen();
							party.horses[i].property = false;
							sprintf(place_string2,"");
							strnum2 = 0;
							i = 1000;
							}
					if (i >= 1000)
						break;
					}
			sprintf(place_string1, "There are no horses left.");
			sprintf(place_string2,"");
			strnum1 = -1;
			strnum2 = -1;
			break;
		case TALK_BUY_SPEC_ITEM:
			if (party.spec_items[a] > 0) {
				sprintf(place_string1, "You already have it.");
				strnum1 = -1;
				}
			else if (party.gold < b) {
				strcpy(place_string1, place_string2);
				strnum1 = strnum2;
				}
				else {
					party.gold -= b;
					put_pc_screen();
					party.spec_items[a] = 1;
					}
			strnum2 = 0;
			sprintf(place_string2,"");
			break;
		case TALK_BUY_JUNK:
			start_shop_mode(SHOP_MAGIC_SHOP_1 + b,0,
				9,a, place_string1);
			strnum1 = -1;
			return;
		case TALK_BUY_TOWN_LOC:
			if (party.can_find_town[b] > 0) {
				}
				else if (party.gold < a) {
					strnum1 = strnum2;
					strcpy(place_string1, place_string2);
					}
					else {
						party.gold -= a;
						put_pc_screen();
						party.can_find_town[b] = 1;
						}
			strnum2 = 0;
			sprintf(place_string2,"");
			break;
		case TALK_END_FORCE:
			talk_end_forced = true;
			break;
		case TALK_END_FIGHT:
			c_town.monst.dudes[store_m_num].attitude = 1;
			c_town.monst.dudes[store_m_num].mobile = 1;
			talk_end_forced = true;
			break;
		case TALK_END_ALARM:
			set_town_status(0);
			talk_end_forced = true;
			break;
		case TALK_END_DIE:
			c_town.monst.dudes[store_m_num].active = 0;
            // Special killing effects
            if (sd_legit(c_town.monst.dudes[store_m_num].monst_start.spec1,c_town.monst.dudes[store_m_num].monst_start.spec2) == true)
                party.stuff_done[c_town.monst.dudes[store_m_num].monst_start.spec1][c_town.monst.dudes[store_m_num].monst_start.spec2] = 1;
			talk_end_forced = true;
			break;
		case TALK_CALL_TOWN_SPEC: // town special
			 run_special(SPEC_TALK,2,a,c_town.p_loc,&s1,&s2,&s3);
			// check s1 & s2 to see if we got diff str, and, if so, munch old strs
			if ((s1 >= 0) || (s2 >= 0)) {
				strnum1 = -1;
				strnum2 = -1;
				sprintf(place_string1,"");
				sprintf(place_string2,"");
				}
			 get_strs(place_string1, place_string2,2,s1,s2);
			 if (s1 >= 0) strnum1 = 2000 + s1;
			 if (s2 >= 0) strnum2 = 2000 + s2;
			 put_pc_screen();
			 put_item_screen(stat_window,0);
			 break;
		case TALK_CALL_SCEN_SPEC: // scen special
			 run_special(SPEC_TALK,0,a,c_town.p_loc,&s1,&s2,&s3);
			// check s1 & s2 to see if we got diff str, and, if so, munch old strs
			if ((s1 >= 0) || (s2 >= 0)) {
				strnum1 = -1;
				strnum2 = -1;
				sprintf(place_string1,"");
				sprintf(place_string2,"");
				}
			 get_strs(place_string1, place_string2,0,s1,s2);
			 if (s1 >= 0) strnum1 = 3000 + s1;
			 if (s2 >= 0) strnum2 = 3000 + s2;
			put_pc_screen();
			put_item_screen(stat_window,0);
			break;
		}

	strcpy(one_back1, old_str1);
	strcpy(one_back2, old_str2);
	strcpy(old_str1, place_string1);
	strcpy(old_str2, place_string2);
	place_talk_str(old_str1, old_str2,0,dummy_rect);
}

void do_sign(short town_num, short which_sign, short sign_type)
//town_num; // Will be 0 - 200 for town, 200 - 290 for outdoors
//short sign_type; // terrain type
{
	char sign_text[256];
	location view_loc;

	view_loc = (is_out()) ? party.p_loc : c_town.p_loc;
	SetCursor(sword_curs);

	cd_create_dialog(1014,mainPtr);

	store_sign_mode = sign_type;
	if (terrain_pic[sign_type] < 1000)
		csp(1014,3,terrain_pic[sign_type]);
		else csp(1014,3,94);

	if (town_num >= 200) {
		town_num -= 200;
		load_outdoors(town_num % scenario.out_width, town_num / scenario.out_width,party.i_w_c.x,party.i_w_c.y,
			1,which_sign + 100,(char *) sign_text);
		}
		else {
			sprintf((char *) sign_text,"%s",data_store->town_strs[120 + which_sign]);
			}
	csit(1014,2,(char *) sign_text);

	while (dialog_not_toast)
		ModalDialog();
	cd_kill_dialog(1014,0);
}




Boolean prefs_event_filter (short item_hit)
{
	Boolean done_yet = false,did_cancel = false;
	short i;
	 RECT windRECT;

		switch (item_hit) {
			case 1:
				done_yet = true;
				dialog_not_toast = false;
				break;

			case 2:
				done_yet = true;
				dialog_not_toast = false;
				did_cancel = true;
				break;

			case 50: case 52: case 54: case 56: case 60: case 18: case 20: case 22: case 24: case 27: case 38: case 40: case 43: case 45:
				cd_set_led(1099,item_hit,1 - cd_get_led(1099,item_hit));
				break;

			case 29:
				cd_set_led(1099,29,1);
				break;

			case 32: case 34: case 36: case 47:
				cd_set_led(1099,32,(item_hit == 32) ? 1 : 0);
				cd_set_led(1099,34,(item_hit == 34) ? 1 : 0);
				cd_set_led(1099,36,(item_hit == 36) ? 1 : 0);
				cd_set_led(1099,47,(item_hit == 47) ? 1 : 0);
				break;

			case 58:
   				cd_set_led(1099,58,1 - cd_get_led(1099,58));
                break;

			default:
				cd_set_led(1099,4 + cur_display_mode,0);
				cur_display_mode = item_hit - 4;
				cd_set_led(1099,4 + cur_display_mode,1);
				break;
			}
	if (done_yet== true) {
		if (did_cancel == false) {
			display_mode = cur_display_mode;
			party.stuff_done[SFD_NO_MAPS] = cd_get_led(1099,18);
			party.stuff_done[SDF_NO_SOUNDS] = cd_get_led(1099,20);
			play_sounds = 1 - party.stuff_done[SDF_NO_SOUNDS];
			party.stuff_done[SDF_NO_FRILLS] = cd_get_led(1099,22);
			party.stuff_done[SDF_ROOM_DESCS_AGAIN] = cd_get_led(1099,24);
			party.stuff_done[SDF_NO_INSTANT_HELP] = cd_get_led(1099,27);
			party.stuff_done[SDF_EASY_MODE] = cd_get_led(1099,38);
			party.stuff_done[SDF_LESS_WANDER_ENC] = cd_get_led(1099,40);
			party.stuff_done[SDF_NO_TER_ANIM] = cd_get_led(1099,43);
			party.stuff_done[SDF_NO_SHORE_FRILLS] = cd_get_led(1099,45);
			party.stuff_done[SDF_NO_TARGET_LINE] = cd_get_led(1099,50);
			party.stuff_done[SDF_LESS_SOUND] = cd_get_led(1099,52);
			play_startup = cd_get_led(1099,54);
			party.stuff_done[SDF_FASTER_BOOM_SPACES] = cd_get_led(1099,56);
			party.stuff_done[SDF_ASK_ABOUT_TEXT_BOX] = cd_get_led(1099,60);

			if(party.stuff_done[SDF_USE_DARKER_GRAPHICS] != cd_get_led(1099,58)){ //changed graphic mode (Mac vs Windows) ?
			 party.stuff_done[SDF_USE_DARKER_GRAPHICS] = cd_get_led(1099,58);//adjust the graphic mode SDF
			 swap_platform_graphics();//swap graphics
		     redraw_screen(0);//redraw screen with new graphics
             clear_map();//redraw map with new graphics
             }

			if (cd_get_led(1099,32) == 1)
				PSD[SDF_GAME_SPEED] = 0;
			if (cd_get_led(1099,34) == 1)
				PSD[SDF_GAME_SPEED] = 1;
			if (cd_get_led(1099,36) == 1)
				PSD[SDF_GAME_SPEED] = 2;
			if (cd_get_led(1099,47) == 1)
				PSD[SDF_GAME_SPEED] = 3;
			if (cd_get_led(1099,29) == 1)
				for (i = 0; i < 120; i++)
					party.help_received[i] = 0;

			if (display_mode < 5)
				max_window(mainPtr);
				else {
					GetWindowRect(GetDesktopWindow(),&windRECT);
					MoveWindow(mainPtr,(windRECT.right - (588 + 10)) / 2,
					(windRECT.bottom - (425 + 52)) / 2 ,
					588  + 10,425 + 52,true);
					}
			}
		save_maps = 1 - party.stuff_done[SFD_NO_MAPS];
		give_delays = party.stuff_done[SDF_NO_FRILLS];
		build_data_file(2);
		}

	return false;
}

void pick_preferences()
{
    get_reg_data();
	cur_display_mode = display_mode;

	SetCursor(sword_curs);

	cd_create_dialog(1099,mainPtr);

	cd_set_led(1099,4 + cur_display_mode,1);

	cd_set_led(1099,18,(party.stuff_done[SFD_NO_MAPS] != 0) ? 1 : 0);
	cd_set_led(1099,20,(play_sounds == false) ? 1 : 0);
	cd_set_led(1099,22,(party.stuff_done[SDF_NO_FRILLS] != 0) ? 1 : 0);
	cd_set_led(1099,24,(party.stuff_done[SDF_ROOM_DESCS_AGAIN] != 0) ? 1 : 0);
	cd_set_led(1099,27,(party.stuff_done[SDF_NO_INSTANT_HELP] != 0) ? 1 : 0);
	cd_set_led(1099,38,(party.stuff_done[SDF_EASY_MODE] != 0) ? 1 : 0);
	cd_set_led(1099,40,(party.stuff_done[SDF_LESS_WANDER_ENC] != 0) ? 1 : 0);
	cd_set_led(1099,43,(party.stuff_done[SDF_NO_TER_ANIM] != 0) ? 1 : 0);
	cd_set_led(1099,45,(party.stuff_done[SDF_NO_SHORE_FRILLS] != 0) ? 1 : 0);
	cd_set_led(1099,50,(party.stuff_done[SDF_NO_TARGET_LINE] != 0) ? 1 : 0);
	cd_set_led(1099,52,(party.stuff_done[SDF_LESS_SOUND] != 0) ? 1 : 0);
	cd_set_led(1099,54,(play_startup != false) ? 1 : 0);
	cd_set_led(1099,56,(party.stuff_done[SDF_FASTER_BOOM_SPACES] != 0) ? 1 : 0);
	cd_set_led(1099,58,(party.stuff_done[SDF_USE_DARKER_GRAPHICS] != 0) ? 1 : 0);
	cd_set_led(1099,60,(party.stuff_done[SDF_ASK_ABOUT_TEXT_BOX] != 0) ? 1 : 0);// talk edit box appearing ?
		cd_set_flag(1099,3,1);
	if (PSD[SDF_GAME_SPEED] == 3)
		cd_set_led(1099,47,1);
		else cd_set_led(1099,32 + PSD[SDF_GAME_SPEED] * 2,1);

	if (party.help_received[55] == 0) {
		cd_initial_draw(1099);
		give_help(55,0,1099);
		}

	while (dialog_not_toast)
		ModalDialog();
	cd_kill_dialog(1099,0);
	adjust_window_mode();
}

Boolean compatibility_event_filter (short item_hit)
{
	Boolean done_yet = false,did_cancel = false;

		switch (item_hit) {
			case 1:
				done_yet = true;
				dialog_not_toast = false;
				break;

			case 2:
				done_yet = true;
				dialog_not_toast = false;
				did_cancel = true;
				break;


			case 7: case 9: case 11: case 13: case 15: case 17: case 19: case 21:
				cd_set_led(1100,item_hit,1 - cd_get_led(1100,item_hit));
				break;
            break;
			}

	if (done_yet== true) {
		if (did_cancel == false) {
			party.stuff_done[SDF_COMPATIBILITY_LEGACY_DAY_REACHED]=cd_get_led(1100,7);
			party.stuff_done[SDF_COMPATIBILITY_LEGACY_KILL_NODE]=1-cd_get_led(1100,9);
			party.stuff_done[SDF_COMPATIBILITY_WORKING_TOWN_WATERFALL]=1-cd_get_led(1100,11);
			party.stuff_done[SDF_COMPATIBILITY_FULL_TRIMS]=cd_get_led(1100,13);
			party.stuff_done[SDF_COMPATIBILITY_SPECIALS_INTERRUPT_REST]=cd_get_led(1100,15);
			party.stuff_done[SDF_COMPATIBILITY_ANYTIME_STAIRWAY_NODES]=cd_get_led(1100,17);
			party.stuff_done[SDF_COMPATIBILITY_CHECK_TIMERS_WHILE_RESTING]=cd_get_led(1100,19);
			party.stuff_done[SDF_COMPATIBILITY_TRIGGER_SPECIALS_ON_BOAT]=cd_get_led(1100,21);
			}
	build_data_file(1);
    }

	return false;
}

void pick_compatibility()
{
    get_reg_data();
	SetCursor(sword_curs);

	cd_create_dialog(1100,mainPtr);

    cd_set_led(1100,7,(party.stuff_done[SDF_COMPATIBILITY_LEGACY_DAY_REACHED]!= 0) ?  1: 0);// party.stuff_done[309][0] legacy_day_reached
    cd_set_led(1100,9,(party.stuff_done[SDF_COMPATIBILITY_LEGACY_KILL_NODE] != 0) ? 0 : 1);// party.stuff_done[309][1] legacy_kill_node
    cd_set_led(1100,11,(party.stuff_done[SDF_COMPATIBILITY_WORKING_TOWN_WATERFALL] != 0) ? 0 : 1);// party.stuff_done[309][2] town_waterfalls
    cd_set_led(1100,13,(party.stuff_done[SDF_COMPATIBILITY_FULL_TRIMS] != 0) ? 1 : 0);// party.stuff_done[309][3] display_grass_trims
    cd_set_led(1100,15,(party.stuff_done[SDF_COMPATIBILITY_SPECIALS_INTERRUPT_REST] != 0) ? 1 : 0);// party.stuff_done[309][4] special_interrupt
    cd_set_led(1100,17,(party.stuff_done[SDF_COMPATIBILITY_ANYTIME_STAIRWAY_NODES] != 0) ? 1 : 0);// party.stuff_done[309][5] stairway_everywhere
    cd_set_led(1100,19,(party.stuff_done[SDF_COMPATIBILITY_CHECK_TIMERS_WHILE_RESTING] != 0) ? 1 : 0);// party.stuff_done[309][6] resting_checks_timers
    cd_set_led(1100,21,(party.stuff_done[SDF_COMPATIBILITY_TRIGGER_SPECIALS_ON_BOAT] != 0) ? 1 : 0);// party.stuff_done[309][7] trigger_special_on_boat

	/*if (party.help_received[70] == 0) {
		cd_initial_draw(1100);
		give_help(70,71,1100);
		}*/

	while (dialog_not_toast)
		ModalDialog();

	cd_kill_dialog(1100,0);
	//adjust_window_mode();
}

void put_party_stats()
{
	short i;

	for (i = 0; i < 6; i++) {
		if (adven[i].main_status > MAIN_STATUS_ABSENT) {
			cd_set_item_text(989,5 + 5 * i,adven[i].name);
			cd_activate_item(989,7 + 5 * i,1);
			cd_activate_item(989,8 + 5 * i,1);
			cd_set_flag(989,6 + 5 * i,96);
			cd_activate_item(989,35 + i,1);
			cd_activate_item(989,42 + i,1);
			csp(989,42 + i,800 + adven[i].which_graphic);
			}
			else {
				cd_set_item_text(989,5 + 5 * i,"Empty.");
				cd_activate_item(989,7 + 5 * i,0);
				cd_activate_item(989,8 + 5 * i,0);
				cd_set_flag(989,6 + 5 * i,98);
				cd_activate_item(989,35 + i,0);
				cd_activate_item(989,42 + i,0);
				}

	}
}

void edit_party_event_filter (short item_hit)
{
	short which_pc;

		switch (item_hit) {
			case 1:
				dialog_not_toast = false;
				break;

			case 41:
				party.help_received[22] = 0;
				give_help(222,23,989);
				return;

			case 5: case 10: case 15: case 20: case 25: case 30:
				which_pc = (item_hit - 5) / 5;
				if (adven[which_pc].main_status != MAIN_STATUS_ABSENT)
					pick_pc_name(which_pc,989);
				put_party_stats();
				break;
			case 7: case 12: case 17: case 22: case 27: case 32:
				which_pc = (item_hit - 7) / 5;
				pick_race_abil(&adven[which_pc],0,989);
				put_party_stats();
				break;
			case 8: case 13: case 18: case 23: case 28: case 33:
				which_pc = (item_hit - 8) / 5;
				spend_xp(which_pc,0,989);
				put_party_stats();
				break;
			case 35: case 36: case 37: case 38: case 39: case 40:
				which_pc = item_hit - 35;

				if (adven[which_pc].main_status <= MAIN_STATUS_ABSENT)
					break;
				if (adven[which_pc].main_status != MAIN_STATUS_ABSENT)
					pick_pc_graphic(which_pc,1,989);
				put_party_stats();
				break;
			case 6: case 11: case 16: case 21: case 26: case 31:
				which_pc = (item_hit - 6) / 5;
				if (adven[which_pc].main_status != MAIN_STATUS_ABSENT) {
					if (FCD(1053,989) == 2)
						adven[which_pc].main_status = MAIN_STATUS_ABSENT;
					put_party_stats();
					}
					else {
						give_help(56,0,989);
						create_pc(which_pc,989);
						put_party_stats();
						cd_initial_draw(989); // extra redraw, just in case
						}
				break;
//			case 7: case 12: case 17: case 22: case 27: case 32:

			}

}

void edit_party()
{

	SetCursor(sword_curs);

	cd_create_dialog(989,mainPtr);

	put_party_stats();
	if (party.help_received[22] == 0) {
		cd_initial_draw(989);
		give_help(22,23,989);
		}
	while (dialog_not_toast)
		ModalDialog();
	cd_kill_dialog(989,0);

	if (adven[current_pc].isAlive() == false)
		current_pc = first_active_pc();


}

void tip_of_day_event_filter (short item_hit)
{
	char place_str[256];

			switch (item_hit) {
				case 1:
					dialog_not_toast = false;
					break;

				case 5:
					store_tip_page_on++;
					if (store_tip_page_on == NUM_HINTS)
						store_tip_page_on = 0;
					GetIndString(place_str,12,50 + store_tip_page_on);
					csit(958,3, place_str);
					break;

				case 7:
					give_intro_hint = 1 - give_intro_hint;
					cd_set_led(958,7,give_intro_hint);
					break;
			}

}

void tip_of_day()
{
	char place_str[256];

	store_tip_page_on = get_ran(1,0,NUM_HINTS - 1);

	SetCursor(sword_curs);

	cd_create_dialog_parent_num(958,0);

	GetIndString(place_str,12,50 + store_tip_page_on);
	csit(958,3, place_str);

	cd_set_led(958,7,give_intro_hint);

	while (dialog_not_toast) ModalDialog();

	cd_kill_dialog(958,0);
    build_data_file(2);
}

void max_window(HWND window)
{
	RECT main_rect,wind_rect;
	short width,height;

	cursor_shown = true;
	showcursor(true);

	GetWindowRect(GetDesktopWindow(),&main_rect);
	GetWindowRect(window,&wind_rect);
	width = main_rect.right - main_rect.left;
	height = main_rect.bottom - main_rect.top;
	MoveWindow(window,0,0,width,height,true);
}

void put_scen_info()
{
	short i;
	char place_str[256];
	char const *ratings[] = {"G","PG","R","NC-17"};
	char const *difficulty[] = {"Low","Medium","High","Very High"};

	for (i = 0; i < 3; i++)
	   if(store_scen_page_on * 3 + i < store_num_scen){
            if(scen_headers[store_scen_page_on * 3 + i].intro_pic == 100){//custom intro pic
           	sprintf(place_str,"scenarios/%s",data_store2[store_scen_page_on * 3 + i].scen_names);
           	place_str[strlen(place_str)-3]='b';
           	place_str[strlen(place_str)-2]='m';
           	place_str[strlen(place_str)-1]='p';
            spec_scen_g = ReadBMP(place_str);
            cd_set_pict(947, 6 + i * 3,2400);
            DeleteObject(spec_scen_g);
            }
            else
			cd_set_pict(947, 6 + i * 3,1600 + scen_headers[store_scen_page_on * 3 + i].intro_pic);
			sprintf((char *) place_str,
				"%s v%d.%d.%d - |  Difficulty: %s, Rating: %s |%s |%s",
				data_store2[store_scen_page_on * 3 + i].scen_header_strs[0],
				(short) scen_headers[store_scen_page_on * 3 + i].ver[0],
				(short) scen_headers[store_scen_page_on * 3 + i].ver[1],
				(short) scen_headers[store_scen_page_on * 3 + i].ver[2],
				difficulty[scen_headers[store_scen_page_on * 3 + i].difficulty],
				ratings[scen_headers[store_scen_page_on * 3 + i].default_ground],
				data_store2[store_scen_page_on * 3 + i].scen_header_strs[1],
				data_store2[store_scen_page_on * 3 + i].scen_header_strs[2]);
			csit(947,7 + i * 3,(char *) place_str);
			cd_activate_item(947,8 + i * 3,1);
			}
			else {
				cd_set_pict(947, 6 + i * 3,950);
				csit(947,7 + i * 3,"");
				cd_activate_item(947,8 + i * 3,0);
				}
}

void pick_a_scen_event_filter (short item_hit)
{
			switch (item_hit) {
				case 1:
					dialog_answer = -1;
					dialog_not_toast = false;
					break;

				case 3: case 4:
					if (item_hit == 3) {
						if (store_scen_page_on == 0)
							store_scen_page_on = (store_num_scen - 1) / 3;
							else store_scen_page_on--;
						}
						else {
							if (store_scen_page_on == (store_num_scen - 1) / 3)
								store_scen_page_on = 0;
								else store_scen_page_on++;
							}
					put_scen_info();
					break;

				case 8: case 11: case 14:
					dialog_answer = ((item_hit - 8) / 3) + store_scen_page_on * 3;
					dialog_not_toast = false;
					break;
				}

}

short pick_a_scen()
{

	build_scen_headers();

	store_scen_page_on = 0;

	if (store_num_scen == 0) {
		FCD(868,0);
		return -1;
		}
	SetCursor(sword_curs);

	cd_create_dialog_parent_num(947,0);

	put_scen_info();

	if (store_num_scen <= 3) {
		cd_activate_item(947,3,0);
		cd_activate_item(947,4,0);
		}
	while (dialog_not_toast)
		ModalDialog();
	cd_kill_dialog(947,0);
	return dialog_answer;
}


void pick_prefab_scen_event_filter (short item_hit)
{
			switch (item_hit) {
				case 1:
					dialog_answer = -1;
					dialog_not_toast = false;
					break;

				case 6: case 9: case 12:
					dialog_answer = ((item_hit - 6) / 3);
					dialog_not_toast = false;
					break;
				}
}

short pick_prefab_scen()
{
	SetCursor(sword_curs);

	cd_create_dialog_parent_num(869,0);
	cd_activate_item(869,2,0);

	while (dialog_not_toast)
		ModalDialog();
	cd_kill_dialog(869,0);
	return dialog_answer;
}
