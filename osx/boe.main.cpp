
//#include "item.h"

#define DIR_ARRAY_DEF
#include "boe.global.h"
#include "classes.h"

#include <thread>
#include "boe.graphics.h"
#include "boe.newgraph.h"
#include "boe.fileio.h"
#include "boe.actions.h"
#include "boe.text.h"
#include "boe.party.h"
#include "boe.items.h"
#include "boe.locutils.h"
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
#include "cursors.h"

extern cursor_type arrow_curs[3][3];
extern cursor_type current_cursor;

/* Mac stuff globals */
bool  All_Done = false;
sf::Event	event;
sf::RenderWindow	mainPtr;
short had_text_freeze = 0,num_fonts;
bool app_started_normally = false, skip_boom_delay = false;
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
bool first_update = true,anim_onscreen = false,frills_on = true,changed_display_mode = false,suppress_stat_screen = false;
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
short which_combat_type;
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

sf::Clock animTimer;

//
//	Main body of program Exile
//

int main(int /*argc*/, char* argv[]) {
#if 0
	void debug_oldstructs();
	debug_oldstructs();
#endif
	try{
		init_directories(argv[0]);
		//data_store = (piles_of_stuff_dumping_type *) NewPtr(sizeof(piles_of_stuff_dumping_type));
		init_menubar(); // Do this first of all because otherwise a default File and Window menu will be seen
		init_graph_tool();
		Initialize();
		init_fileio();
		
		init_buf();
		
		set_up_apple_events();
		//import_template_terrain();
		//import_anim_terrain(0);
		make_cursor_watch();
		plop_fancy_startup();
		
		//PSD[SDF_NO_FRILLS] = 0;
		//PSD[SDF_NO_SOUNDS] = 0;
		
		init_screen_locs();
		
		init_snd_tool();
		cDialog::init();
		
		//init_party(0);
		//PSD[SDF_GAME_SPEED] = 1;
		//init_anim(0);
		
		flushingInput = true;
		
		init_spell_menus();
		
//		if (overall_mode == MODE_STARTUP)
//			overall_mode = MODE_OUTDOORS;
		mainPtr.display();
		
		if (!game_run_before)
			cChoiceDlog("welcome.xml").show();
		else if (give_intro_hint)
			tip_of_day();
		game_run_before = true;
		
		check_for_intel();
		
		init_mini_map();
		menu_activate();
		
		restore_cursor();
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
	text_sbar->setPageSize(11);
	item_sbar.reset(new cScrollbar(mainPtr));
	item_sbar->setBounds(item_sbar_rect);
	item_sbar->setMaximum(16);
	item_sbar->setPageSize(8);
	shop_sbar.reset(new cScrollbar(mainPtr));
	shop_sbar->setBounds(shop_sbar_rect);
	shop_sbar->setMaximum(16);
	shop_sbar->setPageSize(8);
	shop_sbar->hide();
	adjust_window_mode();
	
}

void Handle_One_Event()
{
	static const long twentyTicks = time_in_ticks(20).asMilliseconds();
	static const long fortyTicks = time_in_ticks(40).asMilliseconds();
	
	through_sending();
	Handle_Update();
	
	//(cur_time - last_anim_time > 42)
	if((animTimer.getElapsedTime().asMilliseconds() >= fortyTicks) && (overall_mode != MODE_STARTUP) && (anim_onscreen == true) && (PSD[SDF_NO_TER_ANIM] == 0)
	   && (!gInBackground)) {
		animTimer.restart();
		draw_terrain();
	}
	if((animTimer.getElapsedTime().asMilliseconds() > twentyTicks) && (overall_mode == MODE_STARTUP)
	   && app_started_normally) {
		animTimer.restart();
		draw_startup_anim(true);
	}
	
	clear_sound_memory();
	
	if(map_visible && mini_map.pollEvent(event)){
		if(event.type == sf::Event::Closed) {
			mini_map.setVisible(false);
			map_visible = false;
		} else if(event.type == sf::Event::GainedFocus)
			makeFrontWindow(mainPtr);
	}
	if(!mainPtr.pollEvent(event)) {
		if(changed_display_mode) {
			changed_display_mode = false;
			adjust_window_mode();
		}
		flushingInput = false;
		redraw_screen(REFRESH_NONE);
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
			// Make sure we don't have an arrow cursor when it's outside the window
			make_cursor_sword();
			break;
			
		case sf::Event::MouseMoved:
			if(!gInBackground) {
				location where(event.mouseMove.x, event.mouseMove.y);
				change_cursor(where);
			}
			break;
			
		case sf::Event::GainedFocus:
			Handle_Update();
			makeFrontWindow(mainPtr);
			break;
			
		case sf::Event::Closed:
			if(overall_mode == MODE_STARTUP) {
				if(party_in_memory) {
					std::string choice = cChoiceDlog("quit-confirm-save.xml", {"save","quit","cancel"}).show();
					if(choice == "cancel") break;
					if(choice == "save") {
						fs::path file = nav_put_party();
						if(!file.empty()) break;
						save_party(file);
					}
				}
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
		default:
			break; // There's several events we don't need to handle at all
	}
	flushingInput = false; // TODO: Could there be a case when the key and mouse input that needs to be flushed has other events interspersed?
	mainPtr.display(); // TODO: I'm assuming this needs to be SOMEWHERE, at least.
}


void Handle_Update()
{
	redraw_screen(REFRESH_NONE);
	
	if(map_visible) draw_map(false);
	else mini_map.setVisible(false);
	
}

static void handleUpdateWhileScrolling(volatile bool& doneScrolling, int refresh) {
	while(!doneScrolling) {
		sf::sleep(sf::milliseconds(10));
		redraw_screen(refresh);
		mainPtr.display();
	}
}

// TODO: Pass the event object around instead of keeping a global one
void Mouse_Pressed()
{
	if (had_text_freeze > 0) {
		had_text_freeze--;
		return;
	}
	
	if(overall_mode != MODE_STARTUP) {
		location mousePos(event.mouseButton.x, event.mouseButton.y);
		volatile bool doneScrolling = false;
		if(mousePos.in(text_sbar->getBounds())) {
			std::thread updater(std::bind(handleUpdateWhileScrolling, std::ref(doneScrolling), REFRESH_TRANS));
			text_sbar->handleClick(mousePos);
			doneScrolling = true;
			updater.join();
			redraw_screen(REFRESH_TRANS);
		} else if(mousePos.in(item_sbar->getBounds())) {
			std::thread updater(std::bind(handleUpdateWhileScrolling, std::ref(doneScrolling), REFRESH_INVEN));
			item_sbar->handleClick(mousePos);
			doneScrolling = true;
			updater.join();
			redraw_screen(REFRESH_INVEN);
		} else if(overall_mode == MODE_SHOPPING && mousePos.in(shop_sbar->getBounds())) {
			std::thread updater(std::bind(handleUpdateWhileScrolling, std::ref(doneScrolling), REFRESH_DLOG));
			shop_sbar->handleClick(mousePos);
			doneScrolling = true;
			updater.join();
			redraw_screen(REFRESH_DLOG);
		} else All_Done = handle_action(event);
	} else All_Done = handle_startup_press({event.mouseButton.x, event.mouseButton.y});
	
	menu_activate();
	
}

void close_program()
{
	// TODO: Ultimately we would like to have cleanup happen automatically, negating the need for this function
	//end_music();
	if(univ.town.loaded()) univ.town.unload();
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
			do_load();
			break;
		case 2:
			do_save(0);
			break;
		case 3:
			do_save(1);
			break;
		case 4:
			if(overall_mode != MODE_STARTUP) {
				choice = cChoiceDlog("restart-game.xml",{"okay","cancel"}).show();
				if (choice == "cancel")
					return;
				for (i = 0; i < 6; i++)
					univ.party[i].main_status = eMainStatus::ABSENT;
				party_in_memory = false;
				reload_startup();
				overall_mode = MODE_STARTUP;
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
			
			if(overall_mode == MODE_STARTUP) {
				if(party_in_memory) {
					std::string choice = cChoiceDlog("quit-confirm-save.xml", {"save","quit","cancel"}).show();
					if(choice == "cancel") break;
					if(choice == "save") {
						fs::path file = nav_put_party();
						if(!file.empty()) break;
						save_party(file);
					}
				}
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
						kill_pc(choice,eMainStatus::ABSENT);
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
				if(univ.party[i].main_status == eMainStatus::ABSENT)
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
			if(overall_mode != MODE_STARTUP)
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
			display_alchemy();
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

static cursor_type get_mode_cursor(){
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
		cursor_needed = arrow_curs[cursor_direction.y + 1][cursor_direction.x + 1];
	}
	
	if (cursor_needed != current_cursor)
		set_cursor(cursor_needed);
}

void move_sound(ter_num_t ter,short step){
	static bool on_swamp = false;
	short pic,snd;
	eTerSpec spec;
	
	pic = scenario.ter_types[ter].picture;
	spec = scenario.ter_types[ter].special;
	snd = scenario.ter_types[ter].step_sound;
	
	//if on swamp don't play squish sound : BoE legacy behavior, can be removed safely
	if(snd == 4 && !flying() && univ.party.in_boat == 0){
		if(on_swamp && get_ran(1,1,100) >= 10)return;
		on_swamp = true;
	}else on_swamp = false;
	
	if ((monsters_going == false) && (overall_mode < MODE_COMBAT) && (univ.party.in_boat >= 0)) {// is on boat ?
		if(spec == eTerSpec::TOWN_ENTRANCE)
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
	
	len = (long) length;
	
	if (give_delays == 0)
		sf::sleep(time_in_ticks(len));
}

// TODO: I think this should be in a better place, maybe in cParty?
// stuff done legit, i.e. flags are within proper ranges for stuff done flag
bool sd_legit(short a, short b)
{
	if ((minmax(0,299,a) == a) && (minmax(0,9,b) == b))
		return true;
	return false;
}
