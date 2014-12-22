
//#include <cMemory>
#include <iostream>
#include <cstring>
#include "pc.global.h"
#include "classes.h"
#include "pc.graphics.h"
#include "pc.editors.h"
#include "pc.action.h"
#include "pc.fileio.h"
#include "soundtool.h"
#include "graphtool.h"
#include "boe.consts.h"
#include "dlogutil.h"
#include "fileio.h"
#include "pc.menus.h"
#include "winutil.h"
#include "cursors.h"

cUniverse univ;

rectangle pc_area_buttons[6][4] ; // 0 - whole 1 - pic 2 - name 3 - stat strs 4,5 - later
rectangle item_string_rects[24][4]; // 0 - name 1 - drop  2 - id  3 -
rectangle pc_info_rect; // Frame that holds a pc's basic info and items
rectangle name_rect; //Holds pc name inside pc_info_rect
rectangle info_area_rect;
rectangle hp_sp_rect;   // Holds hit points and spells points for pc
rectangle skill_rect;	// Holds "Skills:" string
rectangle pc_skills_rect[19]; //Holds current pc's  skill levels
rectangle status_rect;  //Holds the string "Status:"
rectangle pc_status_rect[10]; //Holds first 8 effects on pc
rectangle traits_rect; //Holds the string "Traits:"
rectangle pc_traits_rect[16]; //Holds pc traits
rectangle pc_race_rect; //Holds current pc's race
rectangle edit_rect[5][2]; //Buttons that bring up pc edit dialog boxs

short current_active_pc = 0;
//short dialog_answer;

/* Mac stuff globals */
bool All_Done = false,diff_depth_ok = false;
sf::Event event;
sf::RenderWindow mainPtr;
bool gInBackground = false;
fs::path file_in_mem;
bool party_in_scen = false;
bool scen_items_loaded = false;

/* Adventure globals */
//party_record_type party;
//outdoor_record_type outdoors[2][2];
//current_town_type c_town;
//big_tr_type t_d;
//town_item_list	t_i;
//unsigned char out[96][96],out_e[96][96];
//setup_save_type setup_save;
//unsigned char misc_i[64][64],sfx[64][64];
//unsigned char template_terrain[64][64];

short store_flags[3];

//town_record_type anim_town;
//tiny_tr_type anim_t_d;

//stored_items_list_type stored_items[3];
//stored_town_maps_type maps;
//stored_town_maps_type town_maps;
//stored_outdoor_maps_type o_maps;

/* Display globals */
short give_delays = 0; /* XXX this wasn't defined anywhere, is this right? -jmr */

/* Prototypes */
int main(int argc, char* argv[]);
void Initialize(void);
void Handle_One_Event();
void Handle_Activate();
void Handle_Update();
void Mouse_Pressed();
void handle_apple_menu(int item_hit);
void handle_file_menu(int item_hit);
void handle_reg_menu(int item_hit);
void handle_extra_menu(int item_hit);
void handle_edit_menu(int item_hit);
void update_item_menu();
bool verify_restore_quit(bool mode);
void set_up_apple_events();
void handle_item_menu(int item_hit);
//item_record_type convert_item (short_item_record_type s_item);
extern bool cur_scen_is_mac;
extern fs::path progDir;
// File io
short specials_res_id;
char start_name[256];

//
//	Main body of program Exile
//

//MW specified return type was 'void', changed to ISO C style for Carbonisation -jmr
int main(int /*argc*/, char* argv[]) {
	try {
	init_menubar();
	init_directories(argv[0]);
	Initialize();
	init_fileio();
	init_main_buttons();
	Set_up_win();
	init_graph_tool();
	init_snd_tool();
	
	set_up_apple_events();
	
	cDialog::init();
	redraw_screen();
	
	while(!All_Done)
		Handle_One_Event();
		return 0;
	} catch(std::exception& x) {
		giveError(x.what());
		throw;
	} catch(std::string& x) {
		giveError(x);
		throw;
	} catch(...) {
		giveError("An unknown error occurred!");
		throw;
	}
}



//
//	Initialize everything for the program, make sure we can run
//

//MW specified argument and return type.
void Initialize(void) {
	
	check_for_intel();
	
	//
	//	To make the Random sequences truly random, we need to make the seed start
	//	at a different number.  An easy way to do this is to put the current time
	//	and date into the seed.  Since it is always incrementing the starting seed
	//	will always be different.  Don’t for each call of Random, or the sequence
	//	will no longer be random.  Only needed once, here in the init.
	//
	//GetDateTime(&randSeed);
    //  SetQDGlobalsRandomSeed((long)randSeed);
	srand(time(NULL));
	
	//
	//	Make a new window for drawing in, and it must be a color window.
	//	The window is full screen size, made smaller to make it more visible.
	//
	// Size and style obtained from WIND resource #128
	mainPtr.create(sf::VideoMode(590, 440), "Blades of Exile Character Editor", sf::Style::Titlebar | sf::Style::Close);
}

void Handle_One_Event() {
	if(!mainPtr.pollEvent(event)) return;
	
	init_main_buttons();
	redraw_screen();
	
	switch(event.type){
		case sf::Event::KeyPressed:
			break;
			
		case sf::Event::MouseButtonPressed:
			Mouse_Pressed();
			break;
			
		case sf::Event::GainedFocus:
			set_cursor(sword_curs);
			break;
			
		case sf::Event::Closed:
			All_Done = verify_restore_quit(false);
			break;
			
		default:
			break;
	}
}

void Mouse_Pressed() {
	bool try_to_end;
	
	try_to_end = handle_action(event);
	if(try_to_end)
		All_Done = verify_restore_quit(false);
}

void handle_apple_menu(int item_hit) {
	//char desk_acc_name[256];
	//short desk_acc_num;
	
	switch(item_hit) {
		case 1:
			cChoiceDlog("about-pced.xml").show();
			break;
		default:
			//GetItem (apple_menu,item_hit,desk_acc_name);
			//desk_acc_num = OpenDeskAcc(desk_acc_name);
			break;
	}
}

void handle_file_menu(int item_hit) {
	fs::path file;
	
	switch(item_hit) {
		case 1://save
			save_party(file_in_mem, univ);
			break;
		case 2://save as
			file = nav_put_party();
			if(!file.empty()) save_party(file, univ);
			break;
		case 3://open
			if(verify_restore_quit(true)){
				file = nav_get_party();
				if(!file.empty()) {
					if(load_party(file, univ)) {
						file_in_mem = file;
						party_in_scen = !univ.party.scen_name.empty();
						if(!party_in_scen) load_base_item_defs();
						update_item_menu();
					}
				}
				menu_activate();
			}
			break;
		case 7://quit
			All_Done = verify_restore_quit(false);
			break;
	}
}

static void display_strings(short nstr, pic_num_t pic) {
	cStrDlog display_strings(get_str("pcedit", nstr), "", "Editing party", pic, PIC_DLOG);
	display_strings.setSound(57);
	display_strings.show();
}

void handle_extra_menu(int item_hit) {
	short i;
	//cVehicle v_boat = {{12,17},{0,0},{0,0},80,true,false};
	
	if(file_in_mem.empty()) {
		display_strings(5, 7);
		return;
	}
	switch(item_hit) {
		case 1:
			edit_gold_or_food(0);
			break;
		case 2:
			edit_gold_or_food(1);
			break;
			
		case 4:
			if(univ.party.is_split() > 0) {
				cChoiceDlog("reunite-first.xml").show();
				break;
			}
			cChoiceDlog("leave-town.xml").show();
			leave_town();
			break;
			
		case 5:
			if(univ.party.is_split() == 0) {
				cChoiceDlog("not-split.xml").show();
				break;
			}
			cChoiceDlog("reunited.xml").show();
			univ.town.p_loc = univ.party.left_at();
			for(i = 0; i < 6; i++)
				if(univ.party[i].main_status >= eMainStatus::SPLIT)
					univ.party[i].main_status -= eMainStatus::SPLIT;
			break;
			
			
		case 6:
			display_strings(20,7);
			for(i = 0; i < 4; i++)
				univ.party.creature_save[i].which_town = 200;
			break;
		case 8: // damage
			display_strings(1,15);
			for(i = 0; i < 6; i++)
				univ.party[i].cur_health = univ.party[i].max_health;
			break;
		case 9: // spell pts
			display_strings(2,15);
			for(i = 0; i < 6; i++)
				univ.party[i].cur_sp = univ.party[i].max_sp;
			break;
		case 10: // raise dead
			display_strings(3,15);
			for(i = 0; i < 6; i++)
				if(univ.party[i].main_status == eMainStatus::DEAD || univ.party[i].main_status == eMainStatus::DUST ||
						univ.party[i].main_status == eMainStatus::STONE)
					univ.party[i].main_status = eMainStatus::ALIVE;
			break;
		case 11: // conditions
			display_strings(4,15);
			for(i = 0; i < 6; i++) {
				univ.party[i].status[eStatus::POISON] = 0;
				if(univ.party[i].status[eStatus::HASTE_SLOW] < 0)
					univ.party[i].status[eStatus::HASTE_SLOW] = 0;
				univ.party[i].status[eStatus::WEBS] = 0;
				univ.party[i].status[eStatus::DISEASE] = 0;
				univ.party[i].status[eStatus::DUMB] = 0;
				univ.party[i].status[eStatus::ASLEEP] = 0;
				univ.party[i].status[eStatus::PARALYZED] = 0;
				univ.party[i].status[eStatus::ACID] = 0;
			}
			break;
			
		case 13:
			if(!party_in_scen) {
				display_strings(25,15);
				break;
			}
			if(cChoiceDlog("leave-scenario.xml",{"okay","cancel"}).show() != "okay")
				break;
			remove_party_from_scen();
			break;
	}
	redraw_screen();
}

void handle_edit_menu(int item_hit) {
	short i,j,k;
	
	if(file_in_mem.empty()) {
		display_strings(5,7);
		return;
	}
	switch(item_hit) {
		case 1:
			display_alchemy(true);
			break;
		case 2: // all property
			display_strings(6,7);
			for(i = 0; i < 30; i++) {
				univ.party.boats[i].property = false;
				univ.party.horses[i].property = false;
			}
			break;
		case 4: // edit day
			edit_day();
			break;
		case 6: // ouit maps
			if(!party_in_scen) {
				display_strings(25,15);
				break;
			}
			display_strings(13,15);
			for(i = 0; i < 100; i++)
				for(j = 0; j < 6; j++)
					for(k = 0; k < 48; k++)
						univ.out_maps[i][j][k] = 255;
			break;
		case 7: // town maps
			if(!party_in_scen) {
				display_strings(25,15);
				break;
			}
			display_strings(14,15);
			for(i = 0; i < 200; i++)
				for(j = 0; j < 8; j++)
					for(k = 0; k < 64; k++)
						univ.town_maps[i][j][k] = 255;
			break;
		case 9:
			display_pc(current_active_pc,0,0);
			break;
		case 10:
			display_pc(current_active_pc,1,0);
			break;
		case 11:
			pick_race_abil(&univ.party[current_active_pc],0);
			break;
		case 12:
			spend_xp(current_active_pc,1,0);
			break;
		case 13:
			edit_xp(&univ.party[current_active_pc]);
			break;
	}
}

//item_record_type convert_item (short_item_record_type s_item) {
//	item_record_type i;
//	location l = {0,0};
//	short temp_val;
//
//	i.variety = (short) s_item.variety;
//	i.item_level = (short) s_item.item_level;
//	i.awkward = (short) s_item.awkward;
//	i.bonus = (short) s_item.bonus;
//	i.protection = (short) s_item.protection;
//	i.charges = (short) s_item.charges;
//	i.type = (short) s_item.type;
//	i.graphic_num = (short) s_item.graphic_num;
//	if(i.graphic_num >= 25)
//		i.graphic_num += 20;
//	i.ability = (short) s_item.real_abil;
//	i.type_flag = (short) s_item.type_flag;
//	i.is_special = (short) s_item.is_special;
//	i.value = (short) s_item.value;
//	i.weight = s_item.weight;
//	i.special_class = 0;
//	i.item_loc = l;
//	strcpy((char *)i.full_name,(char *)s_item.full_name);
//	strcpy((char *)i.name,(char *)s_item.name);
//
//	if(i.charges > 0)
//		temp_val = i.value * i.charges;
//		else temp_val = i.value;
//	if(temp_val >= 15)
//		i.treas_class = 1;
//	if(temp_val >= 100)
//		i.treas_class = 2;
//	if(temp_val >= 900)
//		i.treas_class = 3;
//	if(temp_val >= 2400)
//		i.treas_class = 4;
//
//	i.magic_use_type = s_item.magic_use_type;
//	i.ability_strength = s_item.ability_strength;
//	i.reserved1 = 0;
//	i.reserved2 = 0;
//	i.item_properties = 0;
//	if(s_item.identified)
//		i.item_properties = i.item_properties | 1;
//	if((s_item.ability == 14) || (s_item.ability == 129) || (s_item.ability == 95))
//		i.item_properties = i.item_properties | 16;
//	if(s_item.magic)
//		i.item_properties = i.item_properties | 4;
//
//	return i;
//}

// TODO: Let this take the item directly instead of the index
void handle_item_menu(int item_hit) {
	cItemRec store_i;
	
	if(file_in_mem.empty()) {
		display_strings(5,7);
		return;
	}
	store_i = univ.scenario.scen_items[item_hit];
	store_i.ident = true;
	give_to_pc(current_active_pc,store_i,false);
}

//void set_cursor(CursHandle which_curs) {
//	HLock ((Handle) which_curs);
//	SetCursor (*which_curs);
//	HUnlock((Handle) which_curs);
//}

//short mode; // 0 - quit  1- restore
bool verify_restore_quit(bool mode) {
	std::string choice;
	
	if(file_in_mem.empty())
		return true;
	cChoiceDlog verify(mode ? "save-open.xml" : "save-quit.xml", {"save", "quit", "cancel"});
	choice = verify.show();
	if(choice == "cancel")
		return false;
	if(choice == "quit")
		return true;
	save_party(file_in_mem, univ);
	return true;
}

//pascal bool cd_event_filter (DialogPtr hDlg, EventRecord *event, short *dummy_item_hit) {
//	char chr,chr2;
//	short the_type,wind_hit,item_hit;
//	Handle the_handle = NULL;
//	rectangle the_rect,button_rect;
//	location the_point;
//	CWindowPtr w;
//	RgnHandle updateRgn;
//
//	dummy_item_hit = 0;
//
//	switch(event->what) {
//		case updateEvt:
//			w = GetDialogWindow(hDlg);
//			updateRgn = NewRgn();
//			GetWindowRegion(w, kWindowUpdateRgn, updateRgn);
//			if(EmptyRgn(updateRgn)) {
//				return true;
//			}
//			BeginUpdate(GetDialogWindow(hDlg));
//			cd_redraw(hDlg);
//			EndUpdate(GetDialogWindow(hDlg));
//			DrawDialog(hDlg);
//			return true;
//			break;
//
//		case keyDown:
//			chr = event->message & charCodeMask;
//			chr2 = (char) ((event->message & keyCodeMask) >> 8);
//			switch(chr2) {
//				case 126: chr = 22; break;
//				case 124: chr = 21; break;
//				case 123: chr = 20; break;
//				case 125: chr = 23; break;
//				case 53: chr = 24; break;
//				case 36: chr = 31; break;
//				case 76: chr = 31; break;
//			}
//			// specials ... 20 - <-  21 - ->  22 up  23 down  24 esc
//			// 25-30  ctrl 1-6  31 - return
//
//			wind_hit = cd_process_keystroke(hDlg,chr,&item_hit);
//			break;
//
//		case mouseDown:
//			the_point = event->where;
//			GlobalToLocal(&the_point);
//			wind_hit = cd_process_click(hDlg,the_point, event->modifiers,&item_hit);
//			break;
//
//		default: wind_hit = -1; break;
//	}
//	switch(wind_hit) {
//		case -1: break;
//
//		case 917: edit_day_event_filter(item_hit); break;
//		case 970: case 971: case 972: case 973: display_strings_event_filter(item_hit); break;
//		case 991: display_pc_event_filter(item_hit); break;
//		case 996: display_alchemy_event_filter(item_hit); break;
//		case 1010: spend_xp_event_filter (item_hit); break;
//		case 1012: case 947: edit_gold_or_food_event_filter (item_hit); break;
//		case 1013: pick_race_abil_event_filter (item_hit); break;
//		case 1018: select_pc_event_filter (item_hit); break;
//		case 1024: edit_xp_event_filter (item_hit); break;
//		case 1073: give_reg_info_event_filter (item_hit); break;
//		default: fancy_choice_dialog_event_filter (item_hit); break;
//	}
//	return(wind_hit != -1);
//}