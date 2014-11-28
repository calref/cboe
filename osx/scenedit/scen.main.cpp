
#include <cstdio>
#include <thread>

#include "scen.global.h"
#include "classes.h"
#include "graphtool.h"
#include "scen.graphics.h"
#include "scen.actions.h"
#include "scen.fileio.h"
#include "scen.btnmg.h"
#include "soundtool.h"
#include "scen.townout.h"
#include "scen.core.h"
#include "scen.keydlgs.h"
#include "mathutil.h"
#include "fileio.h"
#include "scrollbar.h"
#include "winutil.h"
#include "cursors.h"
#include "dlogutil.h"
#include "scen.menus.h"

#include <CoreFoundation/CFByteOrder.h>

cUniverse univ; // not needed; just to silence the compiler

/* Globals */
bool  All_Done = false; // delete play_sounds
sf::Event event;
sf::RenderWindow mainPtr;
cTown* town = NULL;
//big_tr_type t_d;
bool diff_depth_ok = false,mouse_button_held = false,editing_town = false;
short cur_viewing_mode = 0;
//short town_type = 0;  // 0 - big 1 - ave 2 - small
//short max_dim[3] = {64,48,32};
short cen_x, cen_y;
eScenMode overall_mode = MODE_INTRO_SCREEN;
std::shared_ptr<cScrollbar> right_sbar;
short mode_count = 0;
cOutdoors current_terrain;
//cSpeech talking;
//short given_password;
//short user_given_password = -1;
short pixel_depth,old_depth = 8;

ter_num_t border1 = 90, border2 = 90; // kludgy thing ... leave right here, before borders
ter_num_t borders[4][50];

bool change_made = false;

// Numbers of current areas being edited
short cur_town;
location cur_out;

/* Prototypes */
void Initialize(void);
void Handle_One_Event();
void Handle_Activate();
void Handle_Update();
void handle_menu_choice(long choice);
void handle_apple_menu(int item_hit);
void handle_file_menu(int item_hit);
void handle_scenario_menu(int item_hit);
void handle_town_menu(int item_hit);
void handle_outdoor_menu(int item_hit);
void handle_item_menu(int item_hit);
void handle_monst_menu(int item_hit);
void handle_help_menu(int item_hit);
void Mouse_Pressed();
void close_program();
void ding();

cScenario scenario;
//piles_of_stuff_dumping_type *data_store;
RECT right_sbar_rect;
void check_for_intel();
bool mac_is_intel;

// 
//	Main body of program Exileedit
//

//Changed to ISO C specified argument and return type.
int main(int argc, char* argv[]) {
	init_menubar();
	//outdoor_record_type dummy_outdoor, *store2;
	
	
	//data_store = (piles_of_stuff_dumping_type *) NewPtr(sizeof(piles_of_stuff_dumping_type));	
	init_current_terrain();
	//create_file();
	//ExitToShell();
	
	init_directories(argv[0]);
	Initialize();
	init_fileio();
	init_snd_tool();
	load_graphics();
	
	cDialog::init();
	cDialog::defaultBackground = cDialog::BG_LIGHT;
	init_graph_tool();
	
	cen_x = 18;
	cen_y = 18;
	
	run_startup_g();
	init_lb();
	init_rb();
	init_town(1);
	init_out();
	init_scenario();
	
	make_cursor_sword();
	
	Set_up_win();
	init_screen_locs();
	
	//create_basic_scenario();
	shut_down_menus(0);
	
	//update_item_menu();
	
	//	to_create = get_town_to_edit();
	
	//	load_terrain(to_create);
	
	//	modify_lists();
	set_up_start_screen();
	
	check_for_intel();
	redraw_screen();
	
	while (All_Done == false) 
		Handle_One_Event();
	
	close_program();
	return 0;
}

// 
//	Initialize everything for the program, make sure we can run
//

//MW specified argument and return type.
void Initialize(void) {
	
	char tit[256] = "  ";
	
	//
	//	To make the Random sequences truly random, we need to make the seed start
	//	at a different number.  An easy way to do this is to put the current time
	//	and date into the seed.  Since it is always incrementing the starting seed
	//	will always be different.  Don’t for each call of Random, or the sequence
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
	sf::VideoMode mode = sf::VideoMode::getDesktopMode();
	RECT windRect;
	windRect.width() = mode.width;
	windRect.height() = mode.height;
	
	windRect.inset((windRect.right - 584) / 2,(windRect.bottom - 420) / 2);
	windRect.offset(0,18);
	// TODO: I think it should have a close button as well
	mainPtr.create(sf::VideoMode(windRect.width(), windRect.height()), "Blades of Exile Scenario Editor", sf::Style::Titlebar);
	mainPtr.setPosition(windRect.topLeft());
	right_sbar_rect.top = RIGHT_AREA_UL_Y - 1;
	right_sbar_rect.left = RIGHT_AREA_UL_X + RIGHT_AREA_WIDTH - 1 - 16;
	right_sbar_rect.bottom = RIGHT_AREA_UL_Y + RIGHT_AREA_HEIGHT + 1;
	right_sbar_rect.right = RIGHT_AREA_UL_X + RIGHT_AREA_WIDTH - 1;
	right_sbar.reset(new cScrollbar(mainPtr));
	right_sbar->setBounds(right_sbar_rect);
	right_sbar->setPageSize(NRSONPAGE - 1);
	right_sbar->hide();
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

void Handle_One_Event() {
	short chr,chr2;
	long menu_choice;
	
	Handle_Update();
	
	if(!mainPtr.waitEvent(event)) return;
	
	switch (event.type)
	{
		case sf::Event::KeyPressed:
			if(!(event.key.*systemKey))
				handle_keystroke(chr,chr2,event);
			break;
			
		case sf::Event::MouseButtonPressed:
			Mouse_Pressed();
			break;
			
		case sf::Event::MouseMoved:
			if(mouse_button_held)
				handle_action(loc(event.mouseMove.x,event.mouseMove.y),event);
			break;
			
		case sf::Event::MouseButtonReleased:
			mouse_button_held = false;
			break;
	} 
}

void Handle_Update() {
	redraw_screen();
	restore_cursor();
}

//void handle_menu_choice(long choice) {
//	int menu,menu_item;
//	
//	if (choice != 0) {
//		menu = HiWord(choice);
//		menu_item = LoWord(choice);
//		
//		set_cursor(wand_curs);
//		switch (menu) {
//			case 500:
//				handle_apple_menu(menu_item);
//				break;
//			case 550:
//				handle_file_menu(menu_item);
//				break;
//			case 600: 
//				handle_scenario_menu(menu_item);
//				break;
//			case 650:
//				handle_town_menu(menu_item);
//				break;
//			case 651:
//				handle_outdoor_menu(menu_item);
//				break;
//			case 675:
//				handle_help_menu(menu_item);
//				break;
//			case 700: case 701: case 702: case 703: case 704:
//				handle_item_menu(menu_item + 80 * (menu - 700) - 1);
//				break;
//			case 750: case 751: case 752: case 753: 
//				handle_monst_menu(menu_item + 64 * (menu - 750) - 1);
//				break;
//				
//		}
//	} 
//	HiliteMenu(0);
//}

void handle_apple_menu(int item_hit) {
	switch (item_hit) {
		case 1:
			cChoiceDlog("about-scened.xml").show();
			break;
		default:
			//			GetItem (apple_menu,item_hit,desk_acc_name);
			//			desk_acc_num = OpenDeskAcc(desk_acc_name);
			break;
	}
}


void handle_file_menu(int item_hit) {
	fs::path file_to_load;
	switch (item_hit) {
		case 1: // open
				file_to_load = nav_get_scenario();
				if(!file_to_load.empty() && load_scenario(file_to_load)) {
					if(load_town(scenario.last_town_edited,town))
						cur_town = scenario.last_town_edited;
					if(load_outdoors(scenario.last_out_edited,current_terrain)){
						cur_out = scenario.last_out_edited;
						augment_terrain(cur_out);
					}
					overall_mode = MODE_MAIN_SCREEN;
					change_made = false;
					update_item_menu();
					set_up_main_screen();
				}
			break;
		case 2: // save
			modify_lists();
			save_scenario();
			break;
		case 3: // new scen
			build_scenario();
			if (overall_mode == MODE_MAIN_SCREEN)
				set_up_main_screen();
			break;
			
		case 5: // quit
			if(!save_check("save-before-quit.xml"))
				break;
			All_Done = true;
			break;
	}
}

void handle_scenario_menu(int item_hit) {
	short i;
	fs::path file;
	
	switch (item_hit) {
		case 1: // Create new town
			if (change_made == true) {
				giveError("You need to save the changes made to your scenario before you can add a new town.");
				return;
			}
			if (scenario.num_towns >= 200) {
				giveError("You have reached the limit of 200 towns you can have in one scenario.");
				return;
			}
			if (new_town(scenario.num_towns) == true)
				set_up_main_screen();
			break;
		case 3: // Scenario Details
			edit_scen_details();
			break;
		case 4: // Scenario Intro Text
			edit_scen_intro();
			break;
		case 5: // Set Starting Location
			set_starting_loc();
			break;
		case 6: // Change Password
//			if (check_p(user_given_password) == true) {
//				user_given_password = get_password();
//				given_password = true;
//			}
			giveError("Passwords have been disabled; they are no longer necessary.");
			break;
		case 9: // Edit Special Nodes
			right_sbar->setPosition(0);
			start_special_editing(0,0);
			break;
		case 10: // Edit Scenario Text
			right_sbar->setPosition(0);
			start_string_editing(0,0);
			break;
		case 11: // Import Town
			if (change_made == true) {
				giveError("You need to save the changes made to your scenario before you can add a new town.");
				return;
			}
			i = pick_import_town(0,file);
			if (i >= 0) {
				import_town(i,file);
				change_made = true;
				redraw_screen();
			}
			break;
		case 12: // Edit Saved Item Rectangles
			edit_save_rects();
			break;
		case 13: // Edit Horses
			edit_horses();
			break;
		case 14: // Edit Boats
			edit_boats();
			break;
		case 15: // Set Variable Town Entry
			edit_add_town();
			break;
		case 16: // Set Scenario Event Timers
			edit_scenario_events();
			break;
		case 17: // Edit Item Placement Shortcuts
			edit_item_placement();
			break;
		case 18: // Delete Last Town
			if (change_made == true) {
				giveError("You need to save the changes made to your scenario before you can delete a town.");
				return;
			}
			if (scenario.num_towns == 1) {
				giveError("You can't delete the last town in a scenario. All scenarios must have at least 1 town.");
				return;
			}
			if (scenario.num_towns - 1 == cur_town) {
				giveError("You can't delete the last town in a scenario while you're working on it. Load a different town, and try this again.");
				return;
			}
			if (scenario.num_towns - 1 == scenario.which_town_start) {
				giveError("You can't delete the last town in a scenario while it's the town the party starts the scenario in. Change the parties starting point and try this again.");
				return;
			}
			if(cChoiceDlog("delete-town-confirm.xml", {"okay", "cancel"}).show() == "okay")
				delete_last_town();
			break;
		case 19: // Write Data to Text File
			if(cChoiceDlog("data-dump-confirm.xml", {"okay", "cancel"}).show() == "okay")
				start_data_dump();
			break;
		case 20: // Do Full Text Dump
			if(cChoiceDlog("text-dump-confirm.xml", {"okay", "cancel"}).show() == "okay")
				scen_text_dump();
			redraw_screen();
			break;
	}
	if ((item_hit != 18) && (item_hit != 19))
		change_made = true;
}

void handle_town_menu(int item_hit) {
	short i;
	
	change_made = true;
	switch (item_hit) {
		case 1:
			edit_town_details();
			break;
		case 2:
			edit_town_wand();
			break;
		case 3:
			overall_mode = MODE_SET_TOWN_RECT;
			mode_count = 2;
			set_cursor(topleft_curs);
			set_string("Set town boundary","Select upper left corner");
			break;
		case 4:
			frill_up_terrain();
			break;
		case 5:
			unfrill_terrain();
			break;
		case 6:
			edit_town_strs();
			break;
		case 8:
			if(cChoiceDlog("add-random-items.xml", {"okay", "cancel"}).show() == "cancel")
				break;
			place_items_in_town();
			break; // add random
		case 9:
			for (i = 0; i < 64; i++)
				town->preset_items[i].property = 0;
			cChoiceDlog("set-not-owned.xml").show();
			draw_terrain();
			break; // set not prop
		case 10:
			if(cChoiceDlog("clear-items-confirm.xml", {"okay", "cancel"}).show() == "cancel")
				break;
			for (i = 0; i < 64; i++)
				town->preset_items[i].code = -1;
			draw_terrain();
			break; // clear all items
		case 13:
			right_sbar->setPosition(0);
			start_special_editing(2,0);
			break;
		case 14:
			right_sbar->setPosition(0);
			start_string_editing(2,0);
			break;
		case 15:
			edit_advanced_town();
			break;
		case 16:
			edit_town_events();
			break;
	}
}
void handle_outdoor_menu(int item_hit) {
	change_made = true;
	switch (item_hit) {
		case 1:
			outdoor_details();
			break;
		case 2:
			edit_out_wand(0);
			break;
		case 3:
			edit_out_wand(1);
			break;
		case 4:
			frill_up_terrain();
			break;
		case 5:
			unfrill_terrain();
			break;
		case 6:
			edit_out_strs();
			break;
		case 8:
			overall_mode = MODE_SET_OUT_START;
			set_string("Select party starting location.","");
			break;
		case 11:
			right_sbar->setPosition(0);
			start_special_editing(1,0);
			break;
		case 12:
			right_sbar->setPosition(0);
			start_string_editing(1,0);
			break;
	}
}

void handle_help_menu(int item_hit) {
	switch (item_hit) {
		case 1:
			cChoiceDlog("help-editing.xml").show();
			break; // started
		case 2:
			cChoiceDlog("help-testing.xml").show();
			break; // testing
		case 3:
			cChoiceDlog("help-distributing.xml").show();
			break; // distributing
		case 5:
			cChoiceDlog("help-contest.xml").show();
			break; // contest
	}
}

void handle_item_menu(int item_hit) {
	if (scenario.scen_items[item_hit].variety == 0) {
		giveError("This item has its Variety set to No Item. You can only place items with a Variety set to an actual item type.");
		return;
	}
	overall_mode = MODE_PLACE_ITEM;
	set_string("Place the item.","Select item location");
	mode_count = item_hit;
}

void handle_monst_menu(int item_hit) {
	overall_mode = MODE_PLACE_CREATURE;
	set_string("Place the monster.","Select monster location");
	mode_count = item_hit;
}

void handleUpdateWhileScrolling(volatile bool& doneScrolling) {
	while(!doneScrolling) {
		sf::sleep(sf::milliseconds(10));
		// TODO: redraw_screen should probably take the argument specifying what to update
		redraw_screen(/*REFRESH_RIGHT_BAR*/);
		mainPtr.display();
	}
}

void Mouse_Pressed() {
	short	the_part,content_part;
	long menu_choice;
	
	location mousePos(event.mouseButton.x, event.mouseButton.y);
	volatile bool doneScrolling = false;
	if(right_sbar->isVisible() && mousePos.in(right_sbar->getBounds())) {
		std::thread updater(std::bind(handleUpdateWhileScrolling, std::ref(doneScrolling)));
		right_sbar->handleClick(mousePos);
		doneScrolling = true;
		updater.join();
		redraw_screen(/*REFRESH_RIGHT_BAR*/);
	}
	else  // ordinary click
		All_Done = handle_action(loc(event.mouseButton.x,event.mouseButton.y),event);
}

void close_program() {
	if(town != NULL) delete town;
}

void ding() {
	// TODO: Play an error sound here
}
