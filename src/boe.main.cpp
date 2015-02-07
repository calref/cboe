
#define DIR_ARRAY_DEF
#include "boe.global.h"
#include "universe.h"

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
#include "winutil.hpp"
#include "soundtool.hpp"
#include "graphtool.hpp"
#include "mathutil.hpp"
#include "fileio.hpp"
#include "dlogutil.hpp"
#include "scrollbar.hpp"
#include "boe.menus.h"
#include "cursors.hpp"
#include "prefs.hpp"
#include "button.hpp"

extern cursor_type arrow_curs[3][3];
extern cursor_type current_cursor;

bool All_Done = false;
sf::Event event;
sf::RenderWindow mainPtr;
short had_text_freeze = 0,num_fonts;
bool app_started_normally = false, skip_boom_delay = false;
bool first_startup_update = true;
bool diff_depth_ok = false,first_sound_played = false,spell_forced = false,startup_loaded = false;
bool save_maps = true,party_in_memory = false;
std::shared_ptr<cScrollbar> text_sbar, item_sbar, shop_sbar;
std::shared_ptr<cButton> done_btn, help_btn;
rectangle sbar_rect = {283,546,421,562};
rectangle shop_sbar_rect = {67,258,357,274};
rectangle item_sbar_rect = {146,546,253,562};
bool bgm_on = false,bgm_init = false;
location store_anim_ul;
cUniverse univ;

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

/* Display globals */
short combat_posing_monster = -1, current_working_monster = -1; // 0-5 PC 100 + x - monster x
short fast_bang = false; // Note: This mostly behaves as a boolean variable, but values other than 1 are assigned to it, so I made it a short
std::vector<int> spec_item_array;
short current_spell_range;
eGameMode overall_mode = MODE_STARTUP;
bool first_update = true,anim_onscreen = false,frills_on = true,changed_display_mode = false;
short stat_window = 0,store_modifier;
bool monsters_going = false,boom_anim_active = false;
bool give_delays = false;

sf::RenderWindow mini_map;
short which_item_page[6] = {0,0,0,0,0,0}; // Remembers which of the 2 item pages pc looked at
location ul = {28,10};
short display_mode = 0; // 0 - center 1- ul 2 - ur 3 - dl 4 - dr 5 - small win
long stored_key;
short pixel_depth,old_depth = 8;
short current_ground = 0;
eStatMode stat_screen_mode;
short anim_step = -1;

// Spell casting globals
eSpell store_mage = eSpell::LIGHT, store_priest = eSpell::BLESS_MINOR;
short store_mage_lev = 0, store_priest_lev = 0;
short store_spell_target = 6,pc_casting;
short num_targets_left = 0;
location spell_targets[8];

/* Combat globals */
short which_combat_type;
location center;
short current_pc;
short combat_active_pc;
effect_pat_type current_pat;
short spell_caster, missile_firer,current_monst_tactic;
short store_current_pc = 0;

sf::Clock animTimer;

int main(int /*argc*/, char* argv[]) {
#if 0
	void debug_oldstructs();
	debug_oldstructs();
#endif
	try{
		init_directories(argv[0]);
		init_menubar(); // Do this first of all because otherwise a default File and Window menu will be seen
		sync_prefs();
		cUniverse::print_result = iLiving::print_result = add_string_to_buf;
		cPlayer::give_help = give_help;
		init_graph_tool();
		Initialize();
		init_fileio();
		
		init_buf();
		
		set_up_apple_events();
		make_cursor_watch();
		plop_fancy_startup();
		
		init_screen_locs();
		
		init_snd_tool();
		cDialog::init();
		
		flushingInput = true;
		
		init_spell_menus();
		
		mainPtr.display();
		
		if(!game_run_before)
			cChoiceDlog("welcome").show();
		else if(give_intro_hint)
			tip_of_day();
		game_run_before = true;
		
		check_for_intel();
		
		init_mini_map();
		menu_activate();
		
		restore_cursor();
		while(!All_Done)
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

void Initialize(void) {
	//	To make the Random sequences truly random, we need to make the seed start
	//	at a different number.  An easy way to do this is to put the current time
	//	and date into the seed.  Since it is always incrementing the starting seed
	//	will always be different.  Don't for each call of Random, or the sequence
	//	will no longer be random.  Only needed once, here in the init.
	srand(time(NULL));
	
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
	done_btn.reset(new cButton(mainPtr));
	done_btn->setBtnType(BTN_DONE);
	done_btn->hide();
	help_btn.reset(new cButton(mainPtr));
	help_btn->setBtnType(BTN_HELP);
	help_btn->hide();
	adjust_window_mode();
}

void Handle_One_Event() {
	static const long twentyTicks = time_in_ticks(20).asMilliseconds();
	static const long fortyTicks = time_in_ticks(40).asMilliseconds();
	
	through_sending();
	Handle_Update();
	
	//(cur_time - last_anim_time > 42)
	if((animTimer.getElapsedTime().asMilliseconds() >= fortyTicks) && (overall_mode != MODE_STARTUP) && (anim_onscreen) && (PSD[SDF_NO_TER_ANIM] == 0)
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
		return;
	}
	switch(event.type) {
		case sf::Event::KeyPressed:
			if(flushingInput) return;
			if(!(event.key.*systemKey))
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
					std::string choice = cChoiceDlog("quit-confirm-save", {"save","quit","cancel"}).show();
					if(choice == "cancel") break;
					if(choice == "save") {
						fs::path file = nav_put_party();
						if(!file.empty()) break;
						save_party(file, univ);
					}
				}
				All_Done = true;
				break;
			}
			if(overall_mode > MODE_TOWN){
				std::string choice = cChoiceDlog("quit-confirm-nosave", {"quit", "cancel"}).show();
				if(choice == "cancel")
					break;
			}
			else {
				std::string choice = cChoiceDlog("quit-confirm-save", {"save", "quit", "cancel"}).show();
				if(choice == "cancel")
					break;
				if(choice == "save")
					save_party(univ.file, univ);
			}
			All_Done = true;
		default:
			break; // There's several events we don't need to handle at all
	}
	flushingInput = false; // TODO: Could there be a case when the key and mouse input that needs to be flushed has other events interspersed?
}


void Handle_Update() {
	redraw_screen(REFRESH_NONE);
	
	if(map_visible) draw_map(false);
	else mini_map.setVisible(false);
	
}

static void handleUpdateWhileScrolling(volatile bool& doneScrolling, int refresh) {
	while(!doneScrolling) {
		sf::sleep(sf::milliseconds(10));
		redraw_screen(refresh);
	}
}

// TODO: Pass the event object around instead of keeping a global one
void Mouse_Pressed() {
	if(had_text_freeze > 0) {
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

void close_program() {
	// TODO: Ultimately we would like to have cleanup happen automatically, negating the need for this function
	// On the Mac, prefs are synced automatically. However, doing it manually won't hurt.
	// On other platforms, we need to do it manually.
	sync_prefs();
}

void handle_menu_choice(eMenu item_hit) {
	std::string dialogToShow;
	sf::Event dummyEvent = {sf::Event::KeyPressed};
	short i, choice;
	
	switch(item_hit) {
		case eMenu::NONE: break;
		case eMenu::FILE_OPEN:
			do_load();
			break;
		case eMenu::FILE_SAVE:
			do_save(0);
			break;
		case eMenu::FILE_SAVE_AS:
			do_save(1);
			break;
		case eMenu::FILE_NEW:
			if(overall_mode != MODE_STARTUP) {
				std::string choice = cChoiceDlog("restart-game",{"okay","cancel"}).show();
				if(choice == "cancel")
					return;
				for(i = 0; i < 6; i++)
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
		case eMenu::PREFS:
			pick_preferences();
			break;
		case eMenu::QUIT:
			
			if(overall_mode == MODE_STARTUP) {
				if(party_in_memory) {
					std::string choice = cChoiceDlog("quit-confirm-save", {"save","quit","cancel"}).show();
					if(choice == "cancel") break;
					if(choice == "save") {
						fs::path file = nav_put_party();
						if(!file.empty()) break;
						save_party(file, univ);
					}
				}
				All_Done = true;
				break;
			}
			if(overall_mode > MODE_TOWN) {
				std::string choice = cChoiceDlog("quit-confirm-nosave",{"quit","cancel"}).show();
				if(choice == "cancel")
					return;
			}
			else {
				std::string choice = cChoiceDlog("quit-confirm-save",{"quit","save","cancel"}).show();
				if(choice == "cancel")
					break;
				if(choice == "save") {
					if(univ.file.empty()) {
						univ.file = nav_put_party();
						if(univ.file.empty()) break;
					}
					save_party(univ.file, univ);
				}
			}
			All_Done = true;
			break;
		case eMenu::OPTIONS_PC_GRAPHIC:
			choice = char_select_pc(1,"New graphic for who?");
			if(choice < 6)
				pick_pc_graphic(choice,1,NULL);
			draw_terrain();
			break;
			
		case eMenu::OPTIONS_DELETE_PC:
			if(!prime_time()) {
				ASB("Finish what you're doing first.");
				print_buf();
			}
			else {
				choice = char_select_pc(1,"Delete who?");
				if(choice < 6) {
					std::string confirm = cChoiceDlog("delete-pc-confirm",{"yes","no"}).show();
					if(confirm == "yes")
						kill_pc(univ.party[choice],eMainStatus::ABSENT);
				}
				draw_terrain();
			}
			break;
			
			
		case eMenu::OPTIONS_RENAME_PC:
			choice = char_select_pc(1,"Rename who?");
			if(choice < 6)
				pick_pc_name(choice,NULL);
			put_pc_screen();
			put_item_screen(stat_window,0);
			break;
			
			
		case eMenu::OPTIONS_NEW_PC:
			if(!(is_town())) {
				add_string_to_buf("Add PC: Town mode only.");
				print_buf();
				break;
			}
			for(i = 0; i < 6; i++)
				if(univ.party[i].main_status == eMainStatus::ABSENT)
					i = 20;
			if(i == 6) {
				ASB("Add PC: You already have 6 PCs.");
				print_buf();
				break;
			}
			if(univ.town->has_tavern) {
				give_help(56,0);
				create_pc(6,NULL);
			} else {
				add_string_to_buf("Add PC: You cannot add new characters in this town. Try in the town you started in.", 2);
			}
			print_buf();
			put_pc_screen();
			put_item_screen(stat_window,0);
			break;
			
		case eMenu::OPTIONS_JOURNAL:
			journal();
			break;
		case eMenu::OPTIONS_TALK_NOTES:
			if(overall_mode == MODE_TALKING) {
				ASB("Talking notes: Can't read while talking.");
				print_buf();
				return;
			}
			talk_notes();
			break;
		case eMenu::OPTIONS_ENCOUNTER_NOTES:
			adventure_notes();
			break;
		case eMenu::OPTIONS_STATS:
			if(overall_mode != MODE_STARTUP)
				print_party_stats();
			break;
		case eMenu::HELP_OUT:
			dialogToShow = "help-outdoor";
			break;
		case eMenu::HELP_TOWN:
			dialogToShow = "help-town";
			break;
		case eMenu::HELP_COMBAT:
			dialogToShow = "help-combat";
			break;
		case eMenu::HELP_BARRIER:
			dialogToShow = "help-fields";
			break;
		case eMenu::HELP_HINTS:
			dialogToShow = "help-hints";
			break;
		case eMenu::HELP_SPELLS:
			dialogToShow = "help-magic";
			break;
		case eMenu::ABOUT:
			dialogToShow = "about-boe";
			break;
		case eMenu::LIBRARY_MAGE:
			display_spells(eSkill::MAGE_SPELLS,100,0);
			break;
		case eMenu::LIBRARY_PRIEST:
			display_spells(eSkill::PRIEST_SPELLS,100,0);
			break;
		case eMenu::LIBRARY_SKILLS:
			display_skills(eSkill::INVALID,0);
			break;
		case eMenu::LIBRARY_ALCHEMY:
			// TODO: Create a dedicated dialog for alchemy info
			display_alchemy();
			break;
		case eMenu::LIBRARY_TIPS:
			tip_of_day();
			break;
		case eMenu::LIBRARY_INTRO:
			dialogToShow = "welcome";
			break;
		case eMenu::ACTIONS_ALCHEMY:
			dummyEvent.key.code = sf::Keyboard::A;
			dummyEvent.key.shift = true;
			handle_keystroke(dummyEvent);
			break;
		case eMenu::ACTIONS_WAIT:
			dummyEvent.key.code = sf::Keyboard::W;
			handle_keystroke(dummyEvent);
			break;
		case eMenu::ACTIONS_AUTOMAP:
			if(!prime_time()) {
				ASB("Finish what you're doing first.");
				print_buf();
			}
			else {
				give_help(62,0);
				display_map();
			}
			make_cursor_sword();
			break;
		case eMenu::HELP_TOC:
			launchURL("https://calref.net/~sylae/boe-doc/game/Contents.html");
			break;
		case eMenu::ABOUT_MAGE:
		case eMenu::ABOUT_PRIEST:
			give_help(209,0);
			break;
	}
	if(!dialogToShow.empty())
		cChoiceDlog(dialogToShow).show();
}

//  TODO: Let this function take a cMonster* instead of the item_hit
void handle_monster_info_menu(int item_hit) {
	display_monst(item_hit - 1, NULL,1);
}

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

void change_cursor(location where_curs) {
	cursor_type cursor_needed;
	location cursor_direction;
	rectangle world_screen = {23, 23, 346, 274};
	
	where_curs.x -= ul.x;
	where_curs.y -= ul.y;
	
	if(!world_screen.contains(where_curs))
		cursor_needed = sword_curs;
	else cursor_needed = get_mode_cursor();
	
	if((overall_mode == MODE_OUTDOORS || overall_mode == MODE_TOWN || overall_mode == MODE_COMBAT) && world_screen.contains(where_curs)){
		cursor_direction = get_cur_direction(where_curs);
		cursor_needed = arrow_curs[cursor_direction.y + 1][cursor_direction.x + 1];
	}
	
	if(cursor_needed != current_cursor)
		set_cursor(cursor_needed);
}

void move_sound(ter_num_t ter,short step){
	static bool on_swamp = false;
	short pic;
	eTerSpec spec;
	
	pic = univ.scenario.ter_types[ter].picture;
	spec = univ.scenario.ter_types[ter].special;
	eStepSnd snd = univ.scenario.ter_types[ter].step_sound;
	
	// if on swamp don't play squish sound : BoE legacy behavior, can be removed safely
	if(snd == eStepSnd::SPLASH && !flying() && univ.party.in_boat < 0){
		if(on_swamp && get_ran(1,1,100) >= 10) return;
		on_swamp = true;
	} else on_swamp = false;
	
	if(!monsters_going && (overall_mode < MODE_COMBAT) && (univ.party.in_boat >= 0)) {
		if(spec == eTerSpec::TOWN_ENTRANCE)
			return;
		play_sound(48); //play boat sound
	} else if(!monsters_going && (overall_mode < MODE_COMBAT) && (univ.party.in_horse >= 0)) {
		play_sound(85); //so play horse sound
	} else switch(univ.scenario.ter_types[ter].step_sound){
		case eStepSnd::SQUISH:
			play_sound(55);
			break;
		case eStepSnd::CRUNCH:
			play_sound(47);
			break;
		case eStepSnd::NONE:
			break;
		case eStepSnd::SPLASH:
			play_sound(17);
			break;
		case eStepSnd::STEP:
			if(step % 2 == 0) //footsteps alternate sound
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

void pause(short length) {
	long len;
	
	len = (long) length;
	
	// Before pausing, make sure the screen is updated.
	redraw_screen(REFRESH_NONE);
	
	if(give_delays)
		sf::sleep(time_in_ticks(len));
}
