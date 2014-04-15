
//#include "item.h"

#define DIR_ARRAY_DEF
#include "boe.global.h"
#include "classes.h"

#include "boe.graphics.h"
#include "boe.newgraph.h"
#include "boe.fileio.h"
#include "boe.actions.h"
#include "boe.text.h"
#include "boe.party.h"
#include "boe.items.h"
#include "boe.fields.h"
#include "boe.town.h"
#include "boe.startup.h"
#include "boe.dlgutil.h"
#include "boe.infodlg.h"
#include "boe.main.h"
#include "winutil.h"
#include "soundtool.h"
#include "graphtool.h"
#include "mathutil.h"
#include "fileio.h"
#include "dlogutil.h"
#include "scrollbar.h"
#include "boe.menus.h"

#include <CoreFoundation/CFByteOrder.h>

//extern short arrow_curs[3][3];
//extern short sword_curs, boot_curs, drop_curs, target_curs;
//extern short talk_curs, key_curs, look_curs, current_cursor;

/* Mac stuff globals */
RECT	windRect, Drag_Rect;
bool  All_Done = false;
sf::Event	event;
sf::RenderWindow	mainPtr;
short had_text_freeze = 0,num_fonts;
bool in_startup_mode = true,app_started_normally = false, skip_boom_delay = false;
bool first_startup_update = true;
bool diff_depth_ok = false,first_sound_played = false,spell_forced = false,startup_loaded = false;
bool save_maps = true,party_in_memory = false;
std::shared_ptr<cScrollbar> text_sbar, item_sbar, shop_sbar;
RECT sbar_rect = {283,546,421,562};
RECT shop_sbar_rect = {67,258,357,274};
RECT item_sbar_rect = {146,546,253,562};
bool bgm_on = false,bgm_init = false;
//short dialog_answer;
location store_anim_ul;
cScenario scenario;
cUniverse univ;
//piles_of_stuff_dumping_type *data_store;
//talking_record_type talking;

bool gInBackground = false;
bool flushingInput = false;
long start_time;

short on_spell_menu[2][62];
short on_monst_menu[256];
signed char dir_x_dif[9] = {0,1,1,1,0,-1,-1,-1,0};
signed char dir_y_dif[9] = {-1,-1,0,1,1,1,0,-1,0};

extern bool map_visible;
bool game_run_before = false;
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
bool monsters_going = false,boom_anim_active = false;
short give_delays = 0;

sf::RenderWindow mini_map;
//RECT d_rects[80];
////short d_rect_index[80];
//short town_size[3] = {64,48,32};
short which_item_page[6] = {0,0,0,0,0,0}; // Remembers which of the 2 item pages pc looked at
location ul = {28,10};
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

// System key
#ifdef __APPLE__
bool sf::Event::KeyEvent::*const systemKey = &sf::Event::KeyEvent::system;
#else
bool sf::Event::KeyEvent::*const systemKey = &sf::Event::KeyEvent::control;
#endif

sf::Clock animTimer;

// Special stuff booleans
bool web,crate,barrel,fire_barrier,force_barrier,quickfire,force_wall,fire_wall,antimagic,scloud,ice_wall,blade_wall;
bool sleep_field;

// 
//	Main body of program Exile
//
void check_for_intel();
bool mac_is_intel;

int main(void)
{
	try{
		//data_store = (piles_of_stuff_dumping_type *) NewPtr(sizeof(piles_of_stuff_dumping_type));
		init_menubar(); // Do this first of all because otherwise a default File and Window menu will be seen
		Initialize();
		init_fileio();
		init_graph_tool(redraw_screen);
		
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
		cDialog::init();
		
		//init_party(0);
		//PSD[SDF_GAME_SPEED] = 1;
		//init_anim(0);
		
		fancy_startup_delay();
		
		init_spell_menus();
		
		//	if (overall_mode == MODE_STARTUP)
		//		overall_mode = MODE_OUTDOORS;
		mainPtr.display();
		
		if (!game_run_before)
			cChoiceDlog("welcome.xml").show();
		else if (give_intro_hint)
			tip_of_day();
		game_run_before = true;
		
		check_for_intel();
		
		init_mini_map();
		menu_activate();
		
		while (All_Done == false)
			Handle_One_Event();
		
		close_program();
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

void check_for_intel(){
	int response = CFByteOrderGetCurrent();
	if(response == CFByteOrderUnknown){
		printf("Gestalt error\n");
		exit(1);
	}
	if(response == CFByteOrderLittleEndian) mac_is_intel = true;
	else mac_is_intel = false;
}

// 
//	Initialize everything for the program, make sure we can run
//

//MW specified argument and return type.
void Initialize(void)
{
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
	sf::VideoMode screen = sf::VideoMode::getDesktopMode();
	windRect.left = windRect.top = 0;
	windRect.width() = screen.width;
	windRect.height() = screen.height;
	
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
	text_sbar.reset(new cScrollbar(mainPtr));
	text_sbar->setBounds(sbar_rect);
	text_sbar->setMaximum(58);
	text_sbar->setPosition(58);
	item_sbar.reset(new cScrollbar(mainPtr));
	item_sbar->setBounds(item_sbar_rect);
	item_sbar->setMaximum(16);
	shop_sbar.reset(new cScrollbar(mainPtr));
	shop_sbar->setBounds(shop_sbar_rect);
	shop_sbar->setMaximum(16);
	adjust_window_mode();
	
}

// TODO: This may be unneeded?
void Set_Window_Drag_Bdry()
{
	sf::VideoMode screen = sf::VideoMode::getDesktopMode();
	Drag_Rect.left = Drag_Rect.top = 0;
	Drag_Rect.width() = screen.width;
	Drag_Rect.height() = screen.height;
//	Drag_Rect = (**(GrayRgn)).rgnBBox;
	Drag_Rect.left += DRAG_EDGE;
	Drag_Rect.right -= DRAG_EDGE;
	Drag_Rect.bottom -= DRAG_EDGE;
}

void Handle_One_Event()
{
	static const long fiveTicks = time_in_ticks(5).asMilliseconds();
	static const long twentyTicks = time_in_ticks(20).asMilliseconds();
	static const long fortyTicks = time_in_ticks(40).asMilliseconds();
	long menu_choice;
	bool event_in_dialog = false;
	
	through_sending();
	Handle_Update();

	//(cur_time - last_anim_time > 42)
	if((animTimer.getElapsedTime().asMilliseconds() >= fortyTicks) && (in_startup_mode == false) && (anim_onscreen == true) && (PSD[SDF_NO_TER_ANIM] == 0)
		&& (isFrontWindow(mainPtr) || isFrontWindow(mini_map)) && (!gInBackground)) {
		animTimer.restart();
		draw_terrain();
	}
	if((animTimer.getElapsedTime().asMilliseconds() > twentyTicks) && (in_startup_mode == true)
		&& app_started_normally && isFrontWindow(mainPtr)) {
		animTimer.restart();
		draw_startup_anim(true);
	}

	clear_sound_memory();
	
	event_in_dialog = handle_dialog_event();
	
	if (event_in_dialog == false)
		if(map_visible && mini_map.pollEvent(event)){
			if(event.type == sf::Event::Closed) {
				mini_map.setVisible(false);
				map_visible = false;
			} else if(event.type == sf::Event::GainedFocus)
				makeFrontWindow(mainPtr);
		}
		if(!mainPtr.pollEvent(event)) {
			flushingInput = false;
			mainPtr.display();
			return;
		}
	switch (event.type)
	{
		case sf::Event::KeyPressed:
			if(flushingInput) return;
				if (!(event.key.*systemKey))
						handle_keystroke(event);
						
			break;
		
		case sf::Event::MouseButtonPressed:
			if(flushingInput) return;
			Mouse_Pressed();
			break;
			
		case sf::Event::MouseLeft:
			// Make sure we don't have an invisible or arrow cursor when it's outside the window
			// TODO: Would probably be better to reset the cursor to normal rather than making it a sword
			mainPtr.setMouseCursorVisible(true);
			make_cursor_sword();
			break;
			
		case sf::Event::MouseMoved:
			// The original game called ObscureCursor in the handle_keystroke function,
			// which hides the mouse cursor until it's moved.
			// SFML's hide cursor function is always permanent, so this is here to balance that out.
			mainPtr.setMouseCursorVisible(true);
			if(!gInBackground && (isFrontWindow(mainPtr) || isFrontWindow(mini_map))) {
				location where(event.mouseMove.x, event.mouseMove.y);
				change_cursor(where);
				// TODO: Probably don't actually need the conditional here?
				if(animTimer.getElapsedTime().asMilliseconds() % fiveTicks <= 60)
					draw_targeting_line(where);
			}
			break;
		
		case sf::Event::GainedFocus:
			Handle_Update();
			makeFrontWindow(mainPtr);
			break;
			
		case sf::Event::Closed:
			if (in_startup_mode == true) {
				All_Done = true;
				break;
			}
			if (overall_mode > MODE_TOWN){
				std::string choice = cChoiceDlog("quit-confirm-nosave.xml", {"quit", "cancel"}).show();
				if (choice == "cancel")
					break;
			}
			else {
				std::string choice = cChoiceDlog("quit-confirm-save.xml", {"save", "quit", "cancel"}).show();
				if (choice == "cancel")
					break;
				if (choice == "save")
					save_party(univ.file);
			}
			All_Done = true;
			// TODO: Handle closing of mini-map
#if 0
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
#endif
		}
	flushingInput = false; // TODO: Could there be a case when the key and mouse input that needs to be flushed has other events interspersed?
	mainPtr.display(); // TODO: I'm assuming this needs to be SOMEWHERE, at least.
}

// TODO: Not sure what to do here
bool handle_dialog_event() 
{
	bool event_was_dlog = false;
#if 0
	short i,item_hit;
	cDialog* event_d;
	
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
	
#endif
	return event_was_dlog;
}


void Handle_Update()
{
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
	
	if(map_visible) draw_map(false);
	else mini_map.setVisible(false);
		
}

// TODO: These three functions should probably be inside the cScrollbar class (the three are almost identical)
void sbar_action(cScrollbar& bar, short part)
{
	short old_setting,new_setting,max;
	
	if (part == 0)
		return;
	
	old_setting = bar.getPosition();
	new_setting = old_setting;
	max = bar.getMaximum();
	
	switch (part) {
		case 0/*kControlUpButtonPart*/: new_setting--; break;
		case 1/*kControlDownButtonPart*/: new_setting++; break;
		case 2/*kControlPageUpPart*/: new_setting -= 11; break;
		case 3/*kControlPageDownPart*/: new_setting += 11; break;
		}
	new_setting = minmax(0,max,new_setting);
	bar.setPosition(new_setting);
	if (new_setting != old_setting)
		print_buf();
}

void item_sbar_action(cScrollbar& bar, short part)
{
	short old_setting,new_setting;
	short max;
	
	if (part == 0)
		return;
	
	old_setting = bar.getPosition();
	new_setting = old_setting;
	max = bar.getMaximum();
	
	switch (part) {
		case 0/*kControlUpButtonPart*/: new_setting--; break;
		case 1/*kControlDownButtonPart*/: new_setting++; break;
		case 2/*kControlPageUpPart*/: new_setting -= (stat_window == 7) ? 2 : 8; break;
		case 3/*kControlPageDownPart*/: new_setting += (stat_window == 7) ? 2 : 8; break;
		}
	new_setting = minmax(0,max,new_setting);
	bar.setPosition(new_setting);
	if (new_setting != old_setting)
		put_item_screen(stat_window,1);
}

void shop_sbar_action(cScrollbar bar, short part)
{
	short old_setting,new_setting;
	short max;
	
	if (part == 0)
		return;
	
	old_setting = bar.getPosition();
	new_setting = old_setting;
	max = bar.getMaximum();
	
	switch (part) {
		case 0/*kControlUpButtonPart*/: new_setting--; break;
		case 1/*kControlDownButtonPart*/: new_setting++; break;
		case 2/*kControlPageUpPart*/: new_setting -= 8; break;
		case 3/*kControlPageDownPart*/: new_setting += 8; break;
		}
	new_setting = minmax(0,max,new_setting);
	bar.setPosition(new_setting);
	if (new_setting != old_setting)
		draw_shop_graphics(0,shop_sbar_rect);			
}

// TODO: Pass the event object around instead of keeping a global one
void Mouse_Pressed()
{
	// TODO: What about other windows?
//	sf::Window&	the_window = mainPtr;
	short	the_part,choice,i,content_part;
	long menu_choice;
	if (had_text_freeze > 0) {
		had_text_freeze--;
		return;
		}
	
	//ding();
	// TODO: Most of this should be handled automatically by Cocoa, but make sure nothing important was missed (like the code under inGoAway, that looks important)
#if 0
	// TODO: Honestly, I'm not quite sure how to track the scrollbars right now
//					if (the_window == mainPtr) {
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
//					 		else { // ordinary click
#endif // end commented-out code section
								if (in_startup_mode == false)
									All_Done = handle_action(event);
								else All_Done = handle_startup_press({event.mouseButton.x, event.mouseButton.y});

	menu_activate();

}

void close_program()
{
	// TODO: Ultimately we would like to have cleanup happen automatically, negating the need for this function
	//end_music();
	if(univ.town.loaded()) univ.town.unload();
	if(univ.town.cur_talk_loaded != univ.town.num)
		delete univ.town.cur_talk;
	clean_up_graphtool();
}

void handle_apple_menu(int item_hit)
{
	
	switch (item_hit) {
		case 1:
			cChoiceDlog("about-boe.xml").show();
			break;
		default:
//			GetItem (apple_menu,item_hit,desk_acc_name);
//			desk_acc_num = OpenDeskAcc(desk_acc_name);
			break;
		}
}

void handle_file_menu(int item_hit)
{
	std::string choice;
	short i;

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
					do_save(1);
			break;
		case 4:
			if (in_startup_mode == false) {
				choice = cChoiceDlog("restart-game.xml",{"okay","cancel"}).show();
				if (choice == "cancel")
					return;
				for (i = 0; i < 6; i++)
					univ.party[i].main_status = MAIN_STATUS_ABSENT;
				party_in_memory = false;
				reload_startup();
				in_startup_mode = true;
				draw_startup(0);
				}
			start_new_game();
			draw_startup(0);
			menu_activate();
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
				choice = cChoiceDlog("quit-confirm-nosave.xml",{"quit","cancel"}).show();
				if (choice == "cancel")
					return;
				}
				else {
					choice = cChoiceDlog("quit-confirm-save.xml",{"quit","save","cancel"}).show();
					if (choice == "cancel")
						break;
					if (choice == "save")
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
				pick_pc_graphic(choice,1,NULL);
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
						std::string confirm = cChoiceDlog("delete-pc-confirm.xml",{"yes","no"}).show();
						if (confirm == "yes")
							kill_pc(choice,MAIN_STATUS_ABSENT);
						}
					draw_terrain();
					}
			break;
			
			
		case 2:
			choice = select_pc(0,0);
			if (choice < 6)
				pick_pc_name(choice,NULL);
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
				if (univ.party[i].main_status == 0)
					i = 20;
			if (i == 6) {
				ASB("Add PC: You already have 6 PCs.");
				print_buf();
				}
			if (univ.town.num == scenario.which_town_start) {
				give_help(56,0);
				create_pc(6,NULL);
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
	std::string dialogToShow;
	switch (item_hit) {
		case 1: dialogToShow = "help-outdoor.xml"; break;
		case 2: dialogToShow = "help-town.xml"; break;
		case 3: dialogToShow = "help-combat.xml"; break;
		case 4: dialogToShow = "help-fields.xml"; break;
		case 6: dialogToShow = "help-hints.xml"; break;
		case 7: dialogToShow = "help-magic.xml"; break;
		}
	if(!dialogToShow.empty())
		cChoiceDlog(dialogToShow).show();
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
			// TODO: Create a dedicated dialog for alchemy info
				display_alchemy(NULL);
				break;
		case 5:
			tip_of_day(); break;
		case 7:
			cChoiceDlog("welcome.xml").show();
				break;
		}
}

void handle_actions_menu(int item_hit)
{
	sf::Event dummyEvent = {sf::Event::KeyPressed};
	switch (item_hit) {
		case 1:
			dummyEvent.key.code = sf::Keyboard::A;
			dummyEvent.key.shift = true;
			handle_keystroke(dummyEvent);
			break;
		case 2:
			dummyEvent.key.code = sf::Keyboard::W;
			handle_keystroke(dummyEvent);
			break;
		case 3:	
			if (prime_time() == false) {
				ASB("Finish what you're doing first.");
				print_buf();
				}
				else {
					give_help(62,0);
					display_map();
					}
			make_cursor_sword();
			break;
	}
}

// TODO: Maybe just use handle_menu_spell instead
void handle_mage_spells_menu(int item_hit)
{
	switch (item_hit) {
		case 1:
			give_help(209,0);
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
			give_help(209,0);
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

//  TODO: Let this function take a cMonster* instead of the item_hit
void handle_monster_info_menu(int item_hit)
{
	display_monst(item_hit - 1, NULL,1);
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

cursor_type get_mode_cursor(){
	switch(overall_mode){
		case MODE_TOWN_TARGET:
		case MODE_SPELL_TARGET:
		case MODE_FIRING:
		case MODE_THROWING:
		case MODE_FANCY_TARGET:
			return target_curs;
		case MODE_TALK_TOWN:
			return talk_curs;
		case MODE_USE_TOWN:
			return key_curs;
		case MODE_BASH_TOWN:
			return boot_curs;
		case MODE_DROP_TOWN:
		case MODE_DROP_COMBAT:
			return drop_curs;
		case MODE_LOOK_OUTDOORS:
		case MODE_LOOK_TOWN:
		case MODE_LOOK_COMBAT:
			return look_curs;
		case MODE_RESTING:
			return wait_curs;
		default:
			return sword_curs;
	}
	return sword_curs; // this should never be reached, though
}

void change_cursor(location where_curs)
{
	cursor_type cursor_needed;
	location cursor_direction;
	RECT world_screen = {23, 23, 346, 274};
	
	where_curs.x -= ul.x;
	where_curs.y -= ul.y;
	
	if(!world_screen.contains(where_curs))
		cursor_needed = sword_curs;
	else cursor_needed = get_mode_cursor();
	
	if ((overall_mode == MODE_OUTDOORS || overall_mode == MODE_TOWN || overall_mode == MODE_COMBAT) && world_screen.contains(where_curs)){
		cursor_direction = get_cur_direction(where_curs);
		cursor_needed = arrow_curs[cursor_direction.x + 1][cursor_direction.y + 1];// 100 + (cursor_direction.x) + 10 * ( cursor_direction.y);
	}

	if (cursor_needed != current_cursor)
		set_cursor(cursor_needed);
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
		sf::sleep(time_in_ticks(len));
}

// stuff done legit, i.e. flags are within proper ranges for stuff done flag
bool sd_legit(short a, short b)
{
	if ((minmax(0,299,a) == a) && (minmax(0,9,b) == b))
		return true;
	return false;
}
