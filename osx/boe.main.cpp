
//#include "item.h"

#define DIR_ARRAY_DEF
#include "boe.global.h"
#include "classes.h"

#include "gamma.h"
#include "boe.graphics.h"
#include "boe.newgraph.h"
#include "boe.fileio.h"
#include "boe.actions.h"
#include "boe.text.h"
#include "boe.party.h"
#include "boe.items.h"
#include "boe.fields.h"
#include "boe.town.h"
#include "dlgtool.h"
#include "boe.startup.h"
#include "boe.dlgutil.h"
#include "boe.infodlg.h"
#include "boe.main.h"
#include "soundtool.h"
#include "graphtool.h"
#include "mathutil.h"
#include "fileio.h"
#include "dlgutil.h"

//extern short arrow_curs[3][3];
//extern short sword_curs, boot_curs, drop_curs, target_curs;
//extern short talk_curs, key_curs, look_curs, current_cursor;

/* Mac stuff globals */
Rect	windRect, Drag_Rect;
bool  All_Done = false;
EventRecord	event;
WindowPtr	mainPtr;	
Handle menu_bar_handle;
MenuHandle apple_menu,file_menu,extra_menu,help_menu,monster_info_menu,library_menu;
MenuHandle actions_menu,music_menu,mage_spells_menu,priest_spells_menu;
short had_text_freeze = 0,num_fonts;
bool in_startup_mode = true,app_started_normally = false, skip_boom_delay = false;
bool first_startup_update = true;
bool diff_depth_ok = false,first_sound_played = false,spell_forced = false,startup_loaded = false;
bool save_maps = true,party_in_memory = false;
CGrafPtr color_graf_port;
ControlHandle text_sbar = NULL,item_sbar = NULL,shop_sbar = NULL;
Rect sbar_rect = {283,546,421,562};
Rect shop_sbar_rect = {67,258,357,274};
Rect item_sbar_rect = {146,546,253,562};
bool bgm_on = false,bgm_init = false;
//short dialog_answer;
Point store_anim_ul;
cScenario scenario;
cUniverse univ;
//piles_of_stuff_dumping_type *data_store;
//talking_record_type talking;

bool gInBackground = false;
long start_time;

short on_spell_menu[2][62];
short on_monst_menu[256];
signed char dir_x_dif[9] = {0,1,1,1,0,-1,-1,-1,0};
signed char dir_y_dif[9] = {-1,-1,0,1,1,1,0,-1,0};

// Cursors 
extern short current_cursor;

bool game_run_before = false;
bool debug_on = false;
bool give_intro_hint = true;
bool in_scen_debug = false;
bool show_startup_splash = true;
bool belt_present = false;

/* Adventure globals */
//party_record_type party;
//pc_record_type adven[6];
//outdoor_record_type outdoors[2][2];
//current_town_type c_town;
//big_tr_type t_d;
//town_item_list	t_i;
//unsigned char out[96][96],out_e[96][96];
//setup_save_type setup_save;
//unsigned char misc_i[64][64],sfx[64][64];
////unsigned char template_terrain[64][64];
//location monster_targs[60]; // TODO: Integrate this into the town cCreature type
//outdoor_strs_type outdoor_text[2][2];

/* Display globals */
short combat_posing_monster = -1, current_working_monster = -1; // 0-5 PC 100 + x - monster x
bool fast_bang = false;
short spec_item_array[60];
short current_spell_range;
eGameMode overall_mode = MODE_STARTUP;
bool first_update = true,anim_onscreen = false,frills_on = true,sys_7_avail,suppress_stat_screen = false;
short stat_window = 0,store_modifier;
bool monsters_going = false,boom_anim_active = false,cartoon_happening = false;
short give_delays = 0;
bool modeless_exists[18] = {false,false,false,false,false,false,
								false,false,false,false,false,false,
								false,false,false,false,false,false};
short modeless_key[18] = {1079,1080,1081,1082,1084, 1046,1088,1089,1090,1092, 1095,1072,0,0,0,0,0,0};
DialogPtr modeless_dialogs[18] = {NULL,	NULL,	NULL,	NULL,	NULL,	NULL,	
								NULL,	NULL,	NULL,	NULL,	NULL,	NULL,
								NULL,	NULL,	NULL,	NULL,	NULL,	NULL};
//Rect d_rects[80];
////short d_rect_index[80];
//short town_size[3] = {64,48,32};
short which_item_page[6] = {0,0,0,0,0,0}; // Remembers which of the 2 item pages pc looked at
Point ul = {28,10};
short display_mode = 0; // 0 - center 1- ul 2 - ur 3 - dl 4 - dr 5 - small win
long stored_key;
short pixel_depth,old_depth = 8;
short current_ground = 0,stat_screen_mode = 0;
short anim_step = -1;
 
// Spell casting globals
short store_mage = 0, store_priest = 0;
short store_mage_lev = 0, store_priest_lev = 0;
short store_spell_target = 6,pc_casting;
short pc_last_cast[2][6] = {{1,1,1,1,1,1},{1,1,1,1,1,1}};
short num_targets_left = 0;
location spell_targets[8];
					
/* Combat globals */
short which_combat_type,town_type;
location center;
ter_num_t combat_terrain[64][64];
location pc_pos[6];
short current_pc;
short combat_active_pc;
effect_pat_type current_pat;
//short monst_target[60]; // 0-5 target that pc   6 - no target  100 + x - target monster x
// TODO: Integrate this ^ into the town cCreature type
short spell_caster, missile_firer,current_monst_tactic;
short store_current_pc = 0;

////town_record_type anim_town;
//tiny_tr_type anim_t_d;

//stored_items_list_type stored_items[3];
//stored_outdoor_maps_type o_maps;

// Special stuff booleans
bool web,crate,barrel,fire_barrier,force_barrier,quickfire,force_wall,fire_wall,antimagic,scloud,ice_wall,blade_wall;
bool sleep_field;

long last_anim_time = 0;


ModalFilterUPP main_dialog_UPP;

KeyMap key_state;
bool fry_startup = false;

// 
//	Main body of program Exile
//
#ifdef EXILE_BIG_GUNS
//pascal bool cd_event_filter (DialogPtr hDlg, EventRecord *event, short *dummy_item_hit);
ControlActionUPP text_sbar_UPP;
ControlActionUPP item_sbar_UPP;
ControlActionUPP shop_sbar_UPP;
#endif
void check_for_intel();
bool mac_is_intel;



int main(void)
{
	//data_store = (piles_of_stuff_dumping_type *) NewPtr(sizeof(piles_of_stuff_dumping_type));	
	start_time = TickCount();
	Initialize();
#ifdef EXILE_BIG_GUNS
	//main_dialog_UPP = NewModalFilterProc(cd_event_filter);
	text_sbar_UPP = NewControlActionProc(sbar_action);
	item_sbar_UPP = NewControlActionProc(item_sbar_action);
	shop_sbar_UPP = NewControlActionProc(shop_sbar_action);
#endif
	init_fileio();
	init_graph_tool(redraw_screen,&ul);

	Set_Window_Drag_Bdry();

	init_buf();

	set_up_apple_events();
	//import_template_terrain();
	//import_anim_terrain(0);
	plop_fancy_startup();

	//PSD[SDF_NO_FRILLS] = 0;
	//PSD[SDF_NO_SOUNDS] = 0;

	init_screen_locs();
	
	init_snd_tool();
	init_dialogs();
	
	//init_party(0);
	//PSD[SDF_GAME_SPEED] = 1;
	//init_anim(0);

		
	menu_bar_handle = GetNewMBar(128);
	if (menu_bar_handle == NULL) {
		SysBeep(2); SysBeep(2); SysBeep(2);
		ExitToShell();
		}
	SetMenuBar(menu_bar_handle);
	DisposeHandle(menu_bar_handle);


	apple_menu = GetMenuHandle(500);
	file_menu = GetMenuHandle(550);
	extra_menu = GetMenuHandle(600);
	help_menu = GetMenuHandle(650);
	library_menu = GetMenuHandle(750);
	actions_menu = GetMenuHandle(800);
	music_menu = GetMenuHandle(850);

	AppendResMenu(apple_menu, 'DRVR');
	DrawMenuBar();


	menu_activate(0);

	fancy_startup_delay();

	init_spell_menus();

	if (overall_mode == MODE_STARTUP)
		overall_mode = MODE_OUTDOORS;

	if (!game_run_before)
		FCD(986,0);
	else if (give_intro_hint)
		tip_of_day();
	game_run_before = true;
	
	check_for_intel();

	menu_activate(0);
	DrawMenuBar();
	create_modeless_dialog(1046);

	while (All_Done == false) 
		Handle_One_Event();
	
	close_program();
      return 0;
}

void check_for_intel(){
	SInt32 response;
	OSErr err;
	err = Gestalt(gestaltSysArchitecture,&response);
	if(err != noErr){
		printf("Gestalt error %i\n",err);
		exit(1);
	}
	if(response == gestaltIntel) mac_is_intel = true;
	else mac_is_intel = false;
}

// 
//	Initialize everything for the program, make sure we can run
//

//MW specified argument and return type.
void Initialize(void)
{

	Str255 tit = "  ";
		
	/* Initialize all the needed managers. */
	//InitCursor();
	
	
	//GetKeys(key_state);
	
	//
	//	To make the Random sequences truly random, we need to make the seed start
	//	at a different number.  An easy way to do this is to put the current time
	//	and date into the seed.  Since it is always incrementing the starting seed
	//	will always be different.  Don't for each call of Random, or the sequence
	//	will no longer be random.  Only needed once, here in the init.
	//
	//unsigned long time;
	//GetDateTime(&time);
	//SetQDGlobalsRandomSeed(time);
	srand(time(NULL));

	//
	//	Make a new window for drawing in, and it must be a color window.  
	//	The window is full screen size, made smaller to make it more visible.
	//
	BitMap bmap;
	GetQDGlobalsScreenBits(&bmap);
	SetRect(&windRect,bmap.bounds.left,bmap.bounds.top,bmap.bounds.right,bmap.bounds.bottom);
	//windRect = bmap.bounds;

	//find_quickdraw();
	init_directories();

//	stored_key = open_pref_file();
//	if (stored_key == -100) {
//		stored_key = open_pref_file();
//		if (stored_key == -100) {
//			Alert(983,NULL);
//			ExitToShell();	
//		}
//	}
	load_prefs();
	set_pixel_depth();
	mainPtr = GetNewCWindow(128,NULL,IN_FRONT);
	SetPortWindowPort(mainPtr);						/* set window to current graf port */
	text_sbar = NewControl(mainPtr,&sbar_rect,tit,false,58,0,58,scrollBarProc,1);
	item_sbar = NewControl(mainPtr,&item_sbar_rect,tit,false,0,0,16,scrollBarProc,2);
	shop_sbar = NewControl(mainPtr,&shop_sbar_rect,tit,false,0,0,16,scrollBarProc,3);
	adjust_window_mode();
	
}

void Set_Window_Drag_Bdry()
{
	BitMap bmap;
	GetQDGlobalsScreenBits(&bmap);
	Drag_Rect = bmap.bounds;
//	Drag_Rect = (**(GrayRgn)).rgnBBox;
	Drag_Rect.left += DRAG_EDGE;
	Drag_Rect.right -= DRAG_EDGE;
	Drag_Rect.bottom -= DRAG_EDGE;
}

void Handle_One_Event()
{
	short chr,chr2;
	long menu_choice,cur_time;
	bool event_in_dialog = false;
	GrafPtr old_port;
	
	through_sending();
	WaitNextEvent(everyEvent, &event, SLEEP_TICKS, MOUSE_REGION);
	cur_time = TickCount();
	if ((event.what != 23) && (!gInBackground) && 
		((FrontWindow() == mainPtr) || (FrontWindow() == GetDialogWindow(modeless_dialogs[5])))) {
		change_cursor(event.where);
		if ((cur_time - start_time) % 5 == 0)
			draw_targeting_line(event.where);
	}

	//(cur_time - last_anim_time > 42)
	if ((cur_time % 40 == 0) && (in_startup_mode == false) && (anim_onscreen == true) && (PSD[SDF_NO_TER_ANIM] == 0)
		&& ((FrontWindow() == mainPtr) || (FrontWindow() ==GetDialogWindow(modeless_dialogs[5]))) && (!gInBackground)) {
		last_anim_time = cur_time;
		draw_terrain();
	}
	if ((cur_time - last_anim_time > 20) && (in_startup_mode == true) 
		&& (app_started_normally == true) && (FrontWindow() == mainPtr)) {
		last_anim_time = cur_time;
		draw_startup_anim();
	}
		
	GetPort(&old_port);		
//	if (FrontWindow() != old_port)
//		SysBeep(2);

	clear_sound_memory();
	
	event_in_dialog = handle_dialog_event();
	
	if (event_in_dialog == false)
	switch (event.what)
	{
		case keyDown: case autoKey:
				chr = event.message & charCodeMask;
				chr2 = (char) ((event.message & keyCodeMask) >> 8);
				if ((event.modifiers & cmdKey) != 0) {
					if (event.what != autoKey) {
//						SelectWindow(mainPtr);
//						SetPort(mainPtr);
						menu_choice = MenuKey(chr);
						handle_menu_choice(menu_choice);
						}
					}
					else 
						handle_keystroke(chr,chr2,event);
						
			break;
		
		case mouseDown:
			Mouse_Pressed();
			break;
		
		case activateEvt:
			Handle_Activate();
			break;
		
		case updateEvt:
			set_pixel_depth();
			Handle_Update();
			break;

		case kHighLevelEvent: 
			AEProcessAppleEvent(&event);
			break;

		case osEvt:
		/*	1.02 - must BitAND with 0x0FF to get only low byte */
			switch ((event.message >> 24) & 0x0FF) {		/* high byte of message */
				case suspendResumeMessage:		/* suspend/resume is also an activate/deactivate */
					gInBackground = (event.message & 1) == 0;  // 1 is Resume Mask
					//switch_bg_music(gInBackground);
//					DoActivate(FrontWindow(), !gInBackground);
					current_cursor = 300;
					break;
			}
			break;
		}

}


void Handle_Activate()
{
	GrafPtr old_port;
	
	GetPort(&old_port);				
	if (FrontWindow() == mainPtr)
		SetPortWindowPort(mainPtr);
}

bool handle_dialog_event() 
{
	bool event_was_dlog = false;
	short i,item_hit;
	DialogPtr event_d;
	
	if (FrontWindow() != NULL) {
		if (IsDialogEvent(&event)) {
			if (DialogSelect(&event, &event_d, &item_hit))
				for (i = 0; i < 18; i++)
					if (event_d == modeless_dialogs[i])	{
						/*CloseDialog(modeless_dialogs[i]);
						modeless_exists[i] = false;

						event_was_dlog = true;
						
						SetPort(mainPtr);
						SelectWindow(mainPtr);
						SetPort(mainPtr); */
						}
			
			}
	
		}
	
//	if ((event.what == keyDown) && (FrontWindow() != mainPtr))
	
//		}
	
	return event_was_dlog;
}


void Handle_Update()
{
	WindowPtr the_window;
	GrafPtr		old_port;
	
	the_window = (WindowPtr) event.message;
	
	GetPort (&old_port);
	SetPortWindowPort(the_window);
	
	BeginUpdate(the_window);
		
	reset_text_bar(); // Guarantees text bar gets refreshed
	
	if (the_window == mainPtr) {
		if (in_startup_mode == true) {
			/*if (first_startup_update == true) 
				first_startup_update = false;
				else*/ draw_startup(0);
/*			if (first_sound_played == false) {
				play_sound(22);
				first_sound_played = true;
				}  */
			}
		else {
			if (first_update == true) {
				first_update = false;
				if (overall_mode == MODE_OUTDOORS) 
					redraw_screen();
				if ((overall_mode > MODE_OUTDOORS) & (overall_mode < MODE_COMBAT))
					redraw_screen();
			// 1st update never combat
				}
			else //refresh_screen(0); 
				redraw_screen();
			}
		}
		
	EndUpdate(the_window);
	
	SetPort(old_port);
}

pascal void sbar_action(ControlHandle bar, short part)
{
	short old_setting,new_setting,max;
	
	if (part == 0)
		return;
	
	old_setting = GetControlValue(bar);
	new_setting = old_setting;
	max = GetControlMaximum(bar);
	
	switch (part) {
		case kControlUpButtonPart: new_setting--; break;
		case kControlDownButtonPart: new_setting++; break;
		case kControlPageUpPart: new_setting -= 11; break;
		case kControlPageDownPart: new_setting += 11; break;
		}
	new_setting = minmax(0,max,new_setting);
	SetControlValue(bar,new_setting);
	if (new_setting != old_setting)
		print_buf();
}

pascal void item_sbar_action(ControlHandle bar, short part)
{
	short old_setting,new_setting;
	short max;
	
	if (part == 0)
		return;
	
	old_setting = GetControlValue(bar);
	new_setting = old_setting;
	max = GetControlMaximum(bar);
	
	switch (part) {
		case kControlUpButtonPart: new_setting--; break;
		case kControlDownButtonPart: new_setting++; break;
		case kControlPageUpPart: new_setting -= (stat_window == 7) ? 2 : 8; break;
		case kControlPageDownPart: new_setting += (stat_window == 7) ? 2 : 8; break;
		}
	new_setting = minmax(0,max,new_setting);
	SetControlValue(bar,new_setting);
	if (new_setting != old_setting)
		put_item_screen(stat_window,1);
}
pascal void shop_sbar_action(ControlHandle bar, short part)
{
	short old_setting,new_setting;
	short max;
	
	if (part == 0)
		return;
	
	old_setting = GetControlValue(bar);
	new_setting = old_setting;
	max = GetControlMaximum(bar);
	
	switch (part) {
		case kControlUpButtonPart: new_setting--; break;
		case kControlDownButtonPart: new_setting++; break;
		case kControlPageUpPart: new_setting -= 8; break;
		case kControlPageDownPart: new_setting += 8; break;
		}
	new_setting = minmax(0,max,new_setting);
	SetControlValue(bar,new_setting);
	if (new_setting != old_setting)
		draw_shop_graphics(0,shop_sbar_rect);			
}

void Mouse_Pressed()
{
	WindowPtr	the_window;
	short	the_part,choice,i,content_part;
	long menu_choice;
	ControlHandle control_hit;
	BitMap bmap;
	if (had_text_freeze > 0) {
		had_text_freeze--;
		return;
		}
	
	the_part = FindWindow( event.where, &the_window);
	
	//ding();
	
	switch (the_part)
	{
	
		case inMenuBar:
			menu_choice = MenuSelect(event.where);
			handle_menu_choice(menu_choice);
			break;
		
		case inSysWindow:
			break;
		
		case inDrag:
			GetQDGlobalsScreenBits(&bmap);
			DragWindow(the_window, event.where, &(bmap.bounds));
			break;
		
		case inGoAway:
			if (the_window == mainPtr) {
				if (in_startup_mode == true) {
					All_Done = true;
					break;
				}
				if (overall_mode > MODE_TOWN){
					choice = FCD(1067,0);
					if (choice == 1)
						return;
				}
				else {
					choice = FCD(1066,0);
					if (choice == 3)
						break;
					if (choice == 1)
						save_party(univ.file);
				}
				All_Done = true;
			}
			else {
				for (i = 0; i < 18; i++)
					if ((the_window == GetDialogWindow(modeless_dialogs[i])) && (modeless_exists[i] == true)) {
						//CloseDialog(modeless_dialogs[i]);
						HideWindow(GetDialogWindow(modeless_dialogs[i])); 
						modeless_exists[i] = false;
						SelectWindow(mainPtr);
						SetPortWindowPort(mainPtr);		
					}
			}
			break;
		
		case inContent:
			if ((the_window == mainPtr) && (((modeless_exists[5] == false) && (FrontWindow() != the_window)) ||
				((modeless_exists[5] == true) && (the_window == mainPtr) && (FrontWindow() != GetDialogWindow(modeless_dialogs[5]))))) {
				if (modeless_exists[5] == true) {
					SetPortWindowPort(mainPtr);
					SelectWindow(mainPtr);
					SetPortWindowPort(mainPtr);
					BringToFront(GetDialogWindow(modeless_dialogs[5]));
					}
					else {
					SetPortWindowPort(mainPtr);
					SelectWindow(mainPtr);
					SetPortWindowPort(mainPtr);
					}
				}
				else  
					if (the_window == mainPtr) {
						SetPortWindowPort(mainPtr);
						GlobalToLocal(&event.where);
						content_part = FindControl(event.where,the_window,&control_hit); // hit sbar?
						if (content_part != 0) {
							switch (content_part) {
								case kControlIndicatorPart:
									content_part = TrackControl(control_hit,event.where,NULL);
									if (control_hit == text_sbar)
										if (content_part == kControlIndicatorPart)
											print_buf();
									if (control_hit == item_sbar)
										if (content_part == kControlIndicatorPart)
											put_item_screen(stat_window,0);
									if (control_hit == shop_sbar)
										if (content_part == kControlIndicatorPart)
											draw_shop_graphics(0,shop_sbar_rect);			
									break;
								case kControlUpButtonPart: case kControlPageUpPart: case kControlDownButtonPart: case kControlPageDownPart:
#ifndef EXILE_BIG_GUNS
									if (control_hit == text_sbar)
										content_part = TrackControl(control_hit,event.where,(ControlActionUPP)sbar_action);
									if (control_hit == item_sbar)
										content_part = TrackControl(control_hit,event.where,(ControlActionUPP)item_sbar_action);
									if (control_hit == shop_sbar)
										content_part = TrackControl(control_hit,event.where,(ControlActionUPP)shop_sbar_action);
#endif
#ifdef EXILE_BIG_GUNS
									if (control_hit == text_sbar)
										content_part = TrackControl(control_hit,event.where,(ControlActionUPP)text_sbar_UPP);
									if (control_hit == item_sbar)
										content_part = TrackControl(control_hit,event.where,(ControlActionUPP)item_sbar_UPP);
									if (control_hit == shop_sbar)
										content_part = TrackControl(control_hit,event.where,(ControlActionUPP)shop_sbar_UPP);
#endif
									break;

								}
								
							} // a control hit
					 		else { // ordinary click
								if (in_startup_mode == false)
									All_Done = handle_action(event);
								else All_Done = handle_startup_press(event.where);
							}
					}
			break;
	}

	menu_activate((in_startup_mode == true) ? 0 : 1);

}

void close_program()
{
	restore_depth();
	//end_music();
	if(univ.town.loaded()) univ.town.unload();
	if(univ.town.cur_talk != NULL) delete univ.town.cur_talk;
}

void handle_menu_choice(long choice)
{
	int menu,menu_item;

	if (choice != 0) {
		menu = HiWord(choice);
		menu_item = LoWord(choice);

//MenuHandle 500 apple_menu,550 file_menu,600 extra_menu,
//659 help_menu,700 monster_info_menu,750 library_menu,800 actions_menu,850 music_menu;900 mage_spells_menu, 950 priest_spells_menu
		switch (menu) {
			case 500:
				handle_apple_menu(menu_item);
				break;
			case 550:
				handle_file_menu(menu_item);
				break;
			case 600: 
				handle_options_menu(menu_item);
				break;
			case 650:
				handle_help_menu(menu_item);
				break;
			case 700:
				handle_monster_info_menu(menu_item);
				break;
			case 750:
				handle_library_menu(menu_item);
				break;
			case 800:
				handle_actions_menu(menu_item);
				break;
			case 850:
				handle_music_menu(menu_item);
				break;
			case 900:
				handle_mage_spells_menu(menu_item);
				break;
			case 950:
				handle_priest_spells_menu(menu_item);
				break;
			}
		}
	menu_activate((in_startup_mode == true) ? 0 : 1);
 
	HiliteMenu(0);
}

void handle_apple_menu(int item_hit)
{
	
	switch (item_hit) {
		case 1:
			FCD(1062,0);
			break;
		default:
//			GetItem (apple_menu,item_hit,desk_acc_name);
//			desk_acc_num = OpenDeskAcc(desk_acc_name);
			break;
		}
}

void handle_file_menu(int item_hit)
{
	short choice,i;

	switch (item_hit) {
		case 1:
			if (in_startup_mode == true)
				startup_load();
				else do_load();
			break;
		case 2:
			do_save(0);
			break;
		case 3:
//			if (in_startup_mode == true){
//				try{
//					FSSpec file = nav_put_party();
//					save_party(file);
//				} catch(no_file_chosen){}
//			}
//				else
					do_save(1);
			break;
		case 4:
			if (in_startup_mode == false) {
				choice = FCD(1091,0);
				if (choice == 1)
					return;
				for (i = 0; i < 6; i++)
					ADVEN[i].main_status = MAIN_STATUS_ABSENT;
				party_in_memory = false;
				reload_startup();
				in_startup_mode = true;
				draw_startup(0);
				}
			start_new_game();
			update_pc_graphics();
			draw_startup(0);
			break;
		case 6:
			pick_preferences();
			break;
		case 8:

			if (in_startup_mode == true) {
				All_Done = true;
				break;
				}
			if (overall_mode > MODE_TOWN) {
				choice = FCD(1067,0);
				if (choice == 1)
					return;
				}
				else {
					choice = FCD(1066,0);
					if (choice == 3)
						break;
					if (choice == 1)
						save_party(univ.file);
					}
			All_Done = true;
			break;
		}
}

void handle_options_menu(int item_hit)
{
	short choice,i;

	switch (item_hit) {
		case 1:
			choice = char_select_pc(0,0,"New graphic for who?");
			if (choice < 6)
				pick_pc_graphic(choice,1,0);
			update_pc_graphics();
			draw_terrain();
			break;
			
		case 4:
			if (prime_time() == false) {
				ASB("Finish what you're doing first.");
				print_buf();
				}
				else {
					choice = char_select_pc(0,0,"Delete who?");
					if (choice < 6) {
						if ((i = FCD(1053,0)) == 2)
							kill_pc(choice,MAIN_STATUS_ABSENT);
						}
					update_pc_graphics();
					draw_terrain();
					}
			break;
			
			
		case 2:
			choice = select_pc(0,0);
			if (choice < 6)
				pick_pc_name(choice,0);
			put_pc_screen();
			put_item_screen(stat_window,0);
			break;
			
			
		case 3:
			if (!(is_town())) {
				add_string_to_buf("Add PC: Town mode only.");
				print_buf();
				break;
				}
			for (i = 0; i < 6; i++)
				if (ADVEN[i].main_status == 0)
					i = 20;
			if (i == 6) {
				ASB("Add PC: You already have 6 PCs.");
				print_buf();
				}
			if (univ.town.num == scenario.which_town_start) {
				give_help(56,0,0);
				create_pc(6,0);
				}
				else {
					add_string_to_buf("Add PC: You can only make new");
					add_string_to_buf("  characters in the town you ");
					add_string_to_buf("  started in.");
					}
			print_buf();
			put_pc_screen();
			put_item_screen(stat_window,0);
			break;	
			
		//case 6:
		//	journal();
		//	break;////
		case 6:	
			if (overall_mode == MODE_TALKING) {
				ASB("Talking notes: Can't read while talking.");
				print_buf();
				return;
				}
			talk_notes();
			break;
		case 7:
			adventure_notes();
			break;
		case 8:
			if (in_startup_mode == false)
				print_party_stats();
			break;
		}
}

void handle_help_menu(int item_hit)
{
	switch (item_hit) {
		case 1: FCD(1079,0);
				break;
		case 2: FCD(1080,0); break;
		case 3: FCD(1081,0); break;
		case 4: FCD(1072,0); break; // magic barriers
		case 6: FCD(1084,0); break;
		case 7: FCD(1088,0); break;
		}
}
void handle_library_menu(int item_hit)
{
	switch (item_hit) {
		case 1: display_spells(0,100,0);
				break;
		case 2: display_spells(1,100,0);
				break;
		case 3: display_skills(100,0);
			    break;
		case 4:
				display_help(0,0);
				break;
		case 5:
			tip_of_day(); break;
		case 7:
		FCD(986,0);
				break;
		}
}

void handle_actions_menu(int item_hit)
{
	switch (item_hit) {
		case 1:
			handle_keystroke('A',0,event);
			break;
		case 2:
			handle_keystroke('w',0,event);
			break;
		case 3:	
			if (prime_time() == false) {
				ASB("Finish what you're doing first.");
				print_buf();
				}
				else {
					give_help(62,0,0);
					display_map();
					}
			make_cursor_sword();
			break;
	}
}

void handle_mage_spells_menu(int item_hit)
{
	switch (item_hit) {
		case 1:
			give_help(209,0,0);
			break;
		default:
			if (prime_time() == false) {
				ASB("Finish what you're doing first.");
				print_buf();
				}
				else handle_menu_spell(on_spell_menu[0][item_hit - 3],0);
			break;
	}
}
void handle_priest_spells_menu(int item_hit)
{
	switch (item_hit) {
		case 1:
			give_help(209,0,0);
			break;
		default:
			if (prime_time() == false) {
				ASB("Finish what you're doing first.");
				print_buf();
				}
				else handle_menu_spell(on_spell_menu[1][item_hit - 3],1);
			break;
	}
}
void handle_monster_info_menu(int item_hit)
{
	display_monst(item_hit - 1, NULL,1);
}

void handle_music_menu(int item_hit)
{
	short choice;

	switch (item_hit) {
		case 1: 
			if (bgm_on == false) {
				choice = choice_dialog(0,600);
				//if (choice == 1)
				//init_bg_music();
				}
				//else end_music(1);
			if (bgm_on == true)
				PSD[SDF_NO_SHORE_FRILLS] = 1;
				else PSD[SDF_NO_SHORE_FRILLS] = 0;
			break;
		}
}

//void load_cursors()
//{
//	short i,j;
//	for (i = 0; i < 3; i++)
//		for (j = 0; j < 3; j++)
//			arrow_curs[i][j] = GetCursor(100 + (i - 1) + 10 * (j - 1));
//	sword_curs = GetCursor(120);
//
////	HLock ((Handle) sword_curs);
////	SetCursor (*sword_curs);
////	HUnlock((Handle) sword_curs);
//
//	boot_curs = GetCursor(121);
//	key_curs = GetCursor(122);
//	target_curs = GetCursor(124);
//	talk_curs = GetCursor(126);
//	look_curs = GetCursor(129);
//	
//	set_cursor(sword_curs);
//	current_cursor = 124;
//	
//}

//void set_cursor(CursHandle which_curs)
//{
//	HLock ((Handle) which_curs);
//	SetCursor (*which_curs);
//	HUnlock((Handle) which_curs);
//}

void change_cursor(Point where_curs)
{
	short curs_types[50] = {0,0,126,124,122,122,0,0,0,0,
					0,124,124,124,124,122,0,0,0,0,
					120,120,0,0,0,0,0,0,0,0,
					0,0,0,0,0,129,129,129,0,0,
					0,0,0,0,0,120,0,0,0,0},cursor_needed;
	location cursor_direction;
	Rect world_screen = {23, 23, 346, 274};
	

		
	SetPortWindowPort(mainPtr);
	GlobalToLocal(&where_curs);	
	
	where_curs.h -= ul.h;
	where_curs.v -= ul.v;
	
	if (PtInRect(where_curs,&world_screen) == false)
		cursor_needed = 120;
		else cursor_needed = curs_types[overall_mode];
	
	if (cursor_needed == 0) {
		if ((PtInRect(where_curs,&world_screen) == false) || (in_startup_mode == true))
			cursor_needed = 120;
			else {
				cursor_direction = get_cur_direction(where_curs);
				cursor_needed = 100 + (cursor_direction.x) + 10 * ( cursor_direction.y);
				if (cursor_needed == 100)
					cursor_needed = 120;
				}
		}

	if (cursor_needed == current_cursor)
		return;
	
	current_cursor = cursor_needed;
	
	switch (cursor_needed) {
		case 120:
			set_cursor(sword_curs);
			break;
		case 121:
			set_cursor(boot_curs);
			break;
		case 122:
			set_cursor(key_curs);
			break;
		case 124:
			set_cursor(target_curs);
			break;
		case 126:
			set_cursor(talk_curs);
			break;
		case 129:
			set_cursor(look_curs);
			break;
					
		
		default:  // an arrow
			set_cursor(arrow_curs[cursor_direction.x + 1][cursor_direction.y + 1]);
			break;	
		}	
}

void find_quickdraw() {
	OSErr err;
	long response;
	short choice;
	
	err = Gestalt(gestaltQuickdrawVersion, &response);
	if (err == noErr) {
		if (response == 0x000) {
			choice = choice_dialog(0,1070);
			if (choice == 2)
				ExitToShell();
				else diff_depth_ok = true;
			}
		}
		else  {
			SysBeep(50);
			ExitToShell();
			}
}

void set_pixel_depth() {
	GDHandle cur_device;
	PixMapHandle screen_pixmap_handle;

	cur_device = GetGDevice();	
	
	screen_pixmap_handle = (**(cur_device)).gdPMap;
	pixel_depth = (**(screen_pixmap_handle)).pixelSize;
	
	diff_depth_ok = true;
}

void restore_depth()
{
	GDHandle cur_device;
	PixMapHandle screen_pixmap_handle;
	OSErr err;
	
	cur_device = GetGDevice();	

	screen_pixmap_handle = (**(cur_device)).gdPMap;

	if (old_depth != 8) {

			err = SetDepth(cur_device,old_depth,1,1);

		}

}

void check_sys_7()
{
	OSErr err;
	long response;
	
	err = Gestalt(gestaltSystemVersion, &response);
	if ((err == noErr) && (response >= 0x0700))
		sys_7_avail = true;
		else sys_7_avail = false;
}

pascal OSErr handle_open_app(AppleEvent *theAppleEvent,AppleEvent *reply,long handlerRefcon)
//AppleEvent *theAppleEvent,*reply;
//long handlerRefcon;
{
	app_started_normally = true;
	return noErr;
}

pascal OSErr handle_open_doc(AppleEvent *theAppleEvent,AppleEvent *reply,long handlerRefcon)
//AppleEvent *theAppleEvent,*reply;
//long handlerRefcon;
{
	FSSpec myFSS;
	AEDescList docList;
	OSErr myErr;
	long itemsInList;
	Size actualSize;
	AEKeyword keywd;
	DescType returnedType;

	myErr = AEGetParamDesc(theAppleEvent,keyDirectObject, typeAEList, &docList);
	if (myErr == noErr) {
		myErr = AECountItems(&docList,&itemsInList);
		if (myErr == noErr) {	
			myErr = AEGetNthPtr(&docList,1,typeFSS, 
						&keywd,&returnedType,&myFSS,
						sizeof(myFSS),&actualSize);
			if (myErr == noErr) {
				do_apple_event_open(myFSS);
				if ((in_startup_mode == false) && (startup_loaded == true)) 
					end_startup();
				if (in_startup_mode == false) {
					post_load();
					}
					else update_pc_graphics();

				}
			}
		}
	
	
	return noErr;
}

pascal OSErr handle_quit(AppleEvent *theAppleEvent,AppleEvent *reply,long handlerRefcon)
{
	short choice;
	
	if ((overall_mode > MODE_STARTUP/*40*/) || (in_startup_mode == true)) {
		All_Done = true;
		return noErr;
		}

	if (overall_mode < MODE_TALK_TOWN) {
		choice = FCD(1066,0);
		if (choice == 3)
			return userCanceledErr;
		if (choice == 1)
			save_party(univ.file);
		}
		else {
				choice = FCD(1067,0);
				if (choice == 1)
					return userCanceledErr;		
			}
			
	All_Done = true;
	return noErr;
}

void set_up_apple_events()
{
	OSErr myErr;

#ifndef EXILE_BIG_GUNS
	myErr = AEInstallEventHandler(kCoreEventClass,kAEOpenApplication,
		(AEEventHandlerProcPtr) handle_open_app, 0, false);
			
	if (myErr != noErr)
		SysBeep(2);

	myErr = AEInstallEventHandler(kCoreEventClass,kAEOpenDocuments,
		(AEEventHandlerProcPtr) handle_open_doc, 0, false);
			
	if (myErr != noErr)
		SysBeep(2);

	myErr = AEInstallEventHandler(kCoreEventClass,kAEQuitApplication,
		(AEEventHandlerProcPtr) handle_quit, 0, false);
			
	if (myErr != noErr)
		SysBeep(2);
#endif		
#ifdef EXILE_BIG_GUNS
	event_UPP1 = NewAEEventHandlerProc(handle_open_app);
	myErr = AEInstallEventHandler(kCoreEventClass,kAEOpenApplication,
		event_UPP1, 0, false);
			
	if (myErr != noErr)
		SysBeep(2);

	event_UPP2 = NewAEEventHandlerProc(handle_open_doc);
	myErr = AEInstallEventHandler(kCoreEventClass,kAEOpenDocuments,
		event_UPP2, 0, false);
			
	if (myErr != noErr)
		SysBeep(2);

	event_UPP3 = NewAEEventHandlerProc(handle_quit);
	myErr = AEInstallEventHandler(kCoreEventClass,kAEQuitApplication,
		event_UPP3, 0, false);
			
	if (myErr != noErr)
		SysBeep(2);
#endif		

}

void move_sound(ter_num_t ter,short step){
	static bool on_swamp = false;
	short pic,spec,snd;
	
	pic = scenario.ter_types[ter].picture;
	spec = scenario.ter_types[ter].special;
	snd = scenario.ter_types[ter].step_sound;
	
	//if on swamp don't play squish sound : BoE legacy behavior, can be removed safely
	if(snd == 4 && !flying() && univ.party.in_boat == 0){
		if(on_swamp && get_ran(1,1,100) >= 10)return;
		on_swamp = true;
	}else on_swamp = false;
	
	if ((monsters_going == false) && (overall_mode < MODE_COMBAT) && (univ.party.in_boat >= 0)) {// is on boat ?
		if (spec == TER_SPEC_TOWN_ENTRANCE) //town entrance ?
			return;
		play_sound(48); //play boat sound
	}
	else if ((monsters_going == false) && (overall_mode < MODE_COMBAT) && (univ.party.in_horse >= 0)) {//// is on horse ?
		play_sound(85); //so play horse sound
	}
	else switch(scenario.ter_types[ter].step_sound){
		case 1:
			play_sound(55); //squish
			break;
		case 2:
			play_sound(47); //crunch
			break;
		case 3:
			break; //silence : do nothing
		case 4:
			play_sound(17); // big splash
			break;
		default: //safety footsteps valve
			if (step % 2 == 0) //footsteps alternate sound
				play_sound(49);
			else play_sound(50);
	}
}

void incidental_noises(bool on_surface){
	short sound_to_play;
	if(on_surface){
		if(get_ran(1,1,100) < 40){
			sound_to_play = get_ran(1,1,3);
			switch(sound_to_play){
				case 1:
					play_sound(76);
					break;
				case 2:
					play_sound(77);
					break;
				case 3:
					play_sound(91);
					break;
			}
		}
	}else{
		if(get_ran(1,1,100) < 40){
			sound_to_play = get_ran(1,1,2);
			switch(sound_to_play){
				case 1:
					play_sound(78);
					break;
				case 2:
					play_sound(79);
					break;
			}
		}
	}
	// Dog: 80
	// Cat: 81
	// Sheep: 82
	// Cow: 83
	// Chicken: 92
}

void pause(short length)
{
	long len;
	unsigned long dummy;
	
	len = (long) length;
	
	if (give_delays == 0)
		Delay(len, &dummy);
}

// stuff done legit, i.e. flags are within proper ranges for stuff done flag
bool sd_legit(short a, short b)
{
	if ((minmax(0,299,a) == a) && (minmax(0,9,b) == b))
		return true;
	return false;
}
