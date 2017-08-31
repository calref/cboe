
#include <iostream>
#include <cstring>
#include <boost/filesystem/operations.hpp>
#include "universe.hpp"
#include "pc.graphics.hpp"
#include "pc.editors.hpp"
#include "pc.action.hpp"
#include "pc.fileio.hpp"
#include "sounds.hpp"
#include "render_image.hpp"
#include "tiling.hpp"
#include "utility.hpp"
#include "dialog.hpp"
#include "control.hpp"
#include "strdlog.hpp"
#include "choicedlog.hpp"
#include "fileio.hpp"
#include "pc.menus.hpp"
#include "winutil.hpp"
#include "cursors.hpp"
#include "res_image.hpp"

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
rectangle pc_status_rect[15]; //Holds first 8 effects on pc
rectangle traits_rect; //Holds the string "Traits:"
rectangle pc_traits_rect[17]; //Holds pc traits
rectangle pc_race_rect; //Holds current pc's race
rectangle edit_rect[5]; //Buttons that bring up pc edit dialog boxs
std::string scenario_temp_dir_name = "pc_scenario";

short current_active_pc = 0;

/* Mac stuff globals */
bool All_Done = false;
sf::Event event;
sf::RenderWindow mainPtr;
bool gInBackground = false;
fs::path file_in_mem;
bool party_in_scen = false;
bool scen_items_loaded = false;

/* Prototypes */
int main(int argc, char* argv[]);
void Initialize(void);
void Handle_One_Event();
void Handle_Activate();
void Handle_Update();
void Mouse_Pressed();
bool verify_restore_quit(std::string dlog);
void set_up_apple_events(int argc, char* argv[]);
extern bool cur_scen_is_mac;
extern fs::path progDir;
short specials_res_id;
char start_name[256];

//MW specified return type was 'void', changed to ISO C style for Carbonisation -jmr
int main(int argc, char* argv[]) {
	try {
		init_directories(argv[0]);
		init_menubar();
		Initialize();
		init_fileio();
		init_main_buttons();
		Set_up_win();
		init_shaders();
		init_tiling();
		init_snd_tool();
		
		set_up_apple_events(argc, argv);
		
		cDialog::init();
		redraw_screen();
		menu_activate();
		update_item_menu();
		
		while(!All_Done)
			Handle_One_Event();
		return 0;
	} catch(std::exception& x) {
		showFatalError(x.what());
		throw;
	} catch(std::string& x) {
		showFatalError(x);
		throw;
	} catch(...) {
		showFatalError("An unknown error occurred!");
		throw;
	}
}

void Initialize(void) {
	
	check_for_intel();
	
	//
	//	To make the Random sequences truly random, we need to make the seed start
	//	at a different number.  An easy way to do this is to put the current time
	//	and date into the seed.  Since it is always incrementing the starting seed
	//	will always be different.  Don’t for each call of Random, or the sequence
	//	will no longer be random.  Only needed once, here in the init.
	srand(time(nullptr));
	
	//	Make a new window for drawing in, and it must be a color window.
	//	The window is full screen size, made smaller to make it more visible.
	int height = 440 + getMenubarHeight();
	mainPtr.create(sf::VideoMode(590, height), "Blades of Exile Character Editor", sf::Style::Titlebar | sf::Style::Close);
#ifndef __APPLE__ // This overrides Dock icon on OSX, which isn't what we want at all
	ImageRsrc& icon = *ResMgr::get<ImageRsrc>("icon");
	mainPtr.setIcon(icon.getSize().x, icon.getSize().y, icon.copyToImage().getPixelsPtr());
#endif
	init_menubar();
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
			All_Done = verify_restore_quit("save-quit");
			break;
			
		default:
			break;
	}
}

void Mouse_Pressed() {
	bool try_to_end;
	
	try_to_end = handle_action(event);
	if(try_to_end)
		All_Done = verify_restore_quit("save-quit");
}

static void display_strings(short nstr, pic_num_t pic) {
	cStrDlog display_strings(get_str("pcedit", nstr), "", "Editing party", pic, PIC_DLOG);
	display_strings.setSound(57);
	display_strings.show();
}

void handle_menu_choice(eMenu item_hit) {
	fs::path file;
	bool result;
	switch(item_hit) {
		case eMenu::NONE: break;
		case eMenu::ABOUT:
			cChoiceDlog("about-pced").show();
			break;
		case eMenu::FILE_SAVE:
			save_party(file_in_mem, univ);
			break;
		case eMenu::FILE_SAVE_AS:
			file = nav_put_party();
			if(!file.empty()) save_party(file, univ);
			break;
		case eMenu::FILE_OPEN:
			result = verify_restore_quit("save-open");
		if(false)
		case eMenu::FILE_REVERT:
			result = cChoiceDlog("save-revert", {"okay", "cancel"}).show() == "okay";
			if(result) {
				file = item_hit == eMenu::FILE_OPEN ? nav_get_party() : file_in_mem;
				if(!file.empty()) {
					if(load_party(file, univ)) {
						file_in_mem = file;
						party_in_scen = !univ.party.scen_name.empty();
						if(!party_in_scen) load_base_item_defs();
						scen_items_loaded = true;
						update_item_menu();
					}
				}
				menu_activate();
			}
			break;
		case eMenu::FILE_CLOSE:
			if(verify_restore_quit("save-close"))
				file_in_mem = "";
			break;
		case eMenu::QUIT:
			All_Done = verify_restore_quit("save-quit");
			break;
		case eMenu::EDIT_GOLD:
			edit_gold_or_food(0);
			break;
		case eMenu::EDIT_FOOD:
			edit_gold_or_food(1);
			break;
			
		case eMenu::LEAVE_TOWN:
			if(univ.party.is_split()) {
				cChoiceDlog("reunite-first").show();
				break;
			}
			cChoiceDlog("leave-town").show();
			leave_town();
			break;
			
		case eMenu::REUNITE_PARTY:
			if(!univ.party.is_split()) {
				cChoiceDlog("not-split").show();
				break;
			}
			if(univ.party.left_in != size_t(-1) && univ.party.left_in != univ.party.town_num) {
				std::string need_town = univ.scenario.towns[univ.party.left_in]->name;
				showError("You can't reunite your party when you're in a different town than you split up in.", "Return to the following town in-game and then try again: " + need_town);
				break;
			}
			cChoiceDlog("reunited").show();
			univ.party.town_loc = univ.party.left_at;
			for(short i = 0; i < 6; i++)
				if(univ.party[i].main_status >= eMainStatus::SPLIT)
					univ.party[i].main_status -= eMainStatus::SPLIT;
			break;
			
			
		case eMenu::RESET_TOWNS:
			display_strings(20,7);
			for(auto& pop : univ.party.creature_save)
				pop.which_town = 200;
			break;
		case eMenu::HEAL_DAMAGE:
			display_strings(1,15);
			for(short i = 0; i < 6; i++)
				univ.party[i].cur_health = univ.party[i].max_health;
			break;
		case eMenu::RESTORE_MANA:
			display_strings(2,15);
			for(short i = 0; i < 6; i++)
				univ.party[i].cur_sp = univ.party[i].max_sp;
			break;
		case eMenu::RAISE_DEAD:
			display_strings(3,15);
			for(short i = 0; i < 6; i++)
				if(univ.party[i].main_status == eMainStatus::DEAD || univ.party[i].main_status == eMainStatus::DUST ||
						univ.party[i].main_status == eMainStatus::STONE)
					univ.party[i].main_status = eMainStatus::ALIVE;
			break;
		case eMenu::CURE_CONDITIONS:
			display_strings(4,15);
			univ.party.clear_bad_status();
			break;
			
		case eMenu::LEAVE_SCENARIO:
			if(cChoiceDlog("leave-scenario",{"okay","cancel"}).show() != "okay")
				break;
			remove_party_from_scen();
			menu_activate();
			break;
		case eMenu::EDIT_ALCHEMY:
			display_alchemy(true, nullptr);
			break;
		case eMenu::OWN_VEHICLES:
			display_strings(6,7);
			for(short i = 0; i < univ.party.boats.size(); i++)
				univ.party.boats[i].property = false;
			for(short i = 0; i < univ.party.horses.size(); i++)
				univ.party.horses[i].property = false;
			break;
		case eMenu::EDIT_DAY:
			edit_day();
			break;
		case eMenu::ADD_OUT_MAPS:
			display_strings(13,15);
			for(auto sector : univ.scenario.outdoors)
				for(auto& m : sector->maps)
					m.set();
			break;
		case eMenu::ADD_TOWN_MAPS:
			display_strings(14,15);
			for(auto town : univ.scenario.towns)
				for(auto& m : town->maps)
					m.set();
			break;
		case eMenu::EDIT_MAGE:
			display_pc(current_active_pc,10,0);
			break;
		case eMenu::EDIT_PRIEST:
			display_pc(current_active_pc,11,0);
			break;
		case eMenu::EDIT_TRAITS:
			pick_race_abil(&univ.party[current_active_pc],0);
			break;
		case eMenu::EDIT_SKILLS:
			spend_xp(current_active_pc,2,0);
			break;
		case eMenu::EDIT_XP:
			edit_xp(&univ.party[current_active_pc]);
			break;
		case eMenu::SET_SDF:
			edit_stuff_done();
			break;
		case eMenu::HELP_TOC:
			if(fs::is_directory(progDir/"doc"))
				launchURL("file://" + (progDir/"doc/game/Editor.html").string());
			else launchURL("https://blades.calref.net/doc/game/Editor.html");
			break;
	}
}

// TODO: Let this take the item directly instead of the index
void handle_item_menu(cItem& item) {
	cItem store_i = item;
	store_i.ident = true;
	if(!univ.party[current_active_pc].give_item(store_i,GIVE_ALLOW_OVERLOAD))
		showError("Sorry, that PC has no free inventory slots left! You'll have to either drop something or give it to a different PC.");
}

bool verify_restore_quit(std::string dlog) {
	std::string choice;
	
	if(file_in_mem.empty())
		return true;
	cChoiceDlog verify(dlog, {"save", "quit", "cancel"});
	choice = verify.show();
	if(choice == "cancel")
		return false;
	if(choice == "quit")
		return true;
	save_party(file_in_mem, univ);
	return true;
}

// These functions are duplicated here because they're used in the training dialog.
void give_help(short help1, short help2);
void give_help(short help1, short help2, cDialog& parent);
void display_skills(eSkill skill,cDialog* parent);

static void give_help(short help1,short help2,cDialog* parent) {
	bool help_forced = false;
	std::string str1,str2;
	
	if(help1 >= 200) {
		help_forced = true;
		help1 -= 200;
	}
	// This SDF is the "never show instant help" flag
	// TODO: Expose preferences to the PC editor
#if 0
	if(univ.party.stuff_done[306][4] > 0 && !help_forced)
		return;
#endif
	str1 = get_str("help",help1);
	if(help2 > 0)
		str2 = get_str("help",help2);
	cStrDlog display_strings(str1,str2,"Instant Help",24,PIC_DLOG, parent);
	display_strings.setSound(57);
	display_strings.show();
}

void give_help(short help1, short help2) {
	give_help(help1, help2, nullptr);
}

void give_help(short help1, short help2, cDialog& parent) {
	give_help(help1, help2, &parent);
}

void display_skills(eSkill skill,cDialog* parent) {
	extern std::map<eSkill,short> skill_cost;
	extern std::map<eSkill,short> skill_max;
	extern std::map<eSkill,short> skill_g_cost;
	int skill_pos = int(skill);
	cDialog skillDlog("skill-info", parent);
	skillDlog["done"].attachClickHandler(std::bind(&cDialog::toast, &skillDlog, true));
	skillDlog["name"].setText(get_str("skills",skill_pos * 2 + 1));
	skillDlog["skp"].setTextToNum(skill_cost[skill]);
	skillDlog["gold"].setTextToNum(skill_g_cost[skill]);
	skillDlog["max"].setTextToNum(skill_max[skill]);
	skillDlog["desc"].setText(get_str("skills", skill_pos * 2 + 2));
	skillDlog["tips"].setText(get_str("tips", 1 + skill_pos));
	skillDlog["left"].hide();
	skillDlog["right"].hide();
	
	skillDlog.run();
}
