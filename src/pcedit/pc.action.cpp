
#include <cstdio>

#include "pc.graphics.hpp"
#include "universe/universe.hpp"
#include "pc.editors.hpp"
#include "pc.fileio.hpp"
#include "pc.action.hpp"
#include "sounds.hpp"
#include "mathutil.hpp"
#include "dialogxml/dialogs/dialog.hpp"
#include "dialogxml/widgets/control.hpp"
#include "fileio/resmgr/res_dialog.hpp"
#include "tools/cursors.hpp"
#include <boost/lexical_cast.hpp>

extern cUniverse univ;

short which_pc_displayed,store_pc_trait_mode,store_which_to_edit;
extern short current_active_pc;
extern rectangle pc_area_buttons[6][4] ; // 0 - whole 1 - pic 2 - name 3 - stat strs 4,5 - later
extern rectangle item_string_rects[24][4]; // 0 - name 1 - drop  2 - id  3 -
extern rectangle pc_info_rect;
extern rectangle name_rect;

extern rectangle pc_race_rect;
extern rectangle edit_rect[5];

bool check_for_interrupt(std::string); // to suppress "missing prototype" warning
bool check_for_interrupt(std::string) { return false; }

bool handle_action(const sf::Event & event) {
	location the_point = translate_mouse_coordinates({event.mouseButton.x, event.mouseButton.y});
	
	bool to_return = false;
	
	if(univ.file.empty())
		return false;
	
	for(short i = 0; i < 6; i++)
		if((the_point.in(pc_area_buttons[i][0])) &&
		   (univ.party[i].main_status != eMainStatus::ABSENT)) {
			do_button_action(0,i);
			current_active_pc = i;
			redraw_screen();
		}
	for(short i = 0; i < 5; i++)
		if((the_point.in(edit_rect[i])) &&
		   (univ.party[current_active_pc].main_status != eMainStatus::ABSENT)) {
			do_button_action(0,i + 10);
			switch(i) {
				case 0:
					display_pc(current_active_pc,10,nullptr);
					break;
				case 1:
			 		display_pc(current_active_pc,11,nullptr);
					break;
				case 2:
					pick_race_abil(&univ.party[current_active_pc],0);
					break;
				case 3:
					spend_xp(current_active_pc,2,nullptr);
					break;
				case 4:
					edit_xp(&univ.party[current_active_pc]);
					
					break;
			}
		}
	for(short i = 0; i < cPlayer::INVENTORY_SIZE; i++) {
		if((the_point.in(item_string_rects[i][1])) && // drop item
		   univ.party[current_active_pc].items[i].variety != eItemType::NO_ITEM) {
			flash_rect(item_string_rects[i][1]);
			univ.party[current_active_pc].take_item(i);
		}
		if((the_point.in(item_string_rects[i][2])) && // identify item
		   univ.party[current_active_pc].items[i].variety != eItemType::NO_ITEM) {
			flash_rect(item_string_rects[i][2]);
			univ.party[current_active_pc].items[i].ident = true;
		}
	}
	
	return to_return;
}

void flash_rect(rectangle /*to_flash*/) {
	
	// TODO: Think of a good way to do this
	//InvertRect (&to_flash);
	play_sound(37);
	sf::sleep(time_in_ticks(5));
}

static bool get_num_event_filter(cDialog& me, std::string id, eKeyMod) {
	me.toast(id == "okay");
	me.setResult<long long>(me["number"].getTextAsNum());
	return true;
}

//0 - gold 1 - food
void edit_gold_or_food(short which_to_edit) {
	
	location view_loc;
	
	store_which_to_edit = which_to_edit;
	
	set_cursor(sword_curs);
	cDialog dlog(*ResMgr::dialogs.get("get-num"));
	dlog["okay"].attachClickHandler(get_num_event_filter);
	if(which_to_edit == 0)
		dlog["prompt"].setText("How much gold do you want?");
	else dlog["prompt"].setText("How much food do you want?");
	dlog["number"].setTextToNum((which_to_edit == 0) ? univ.party.gold : univ.party.food);
	
	dlog.run();
	int dialog_answer = minmax(0,which_to_edit == 0 ? MAX_GOLD : MAX_FOOD,dlog.getResult<long long>());
	if(which_to_edit == 0)
		univ.party.gold = dialog_answer;
	else
		univ.party.food = dialog_answer;
}

void edit_day() {
	
	location view_loc;
	
	
	set_cursor(sword_curs);
	
	cDialog dlog(*ResMgr::dialogs.get("edit-day"));
	dlog["okay"].attachClickHandler(get_num_event_filter);
	
	dlog["number"].setTextToNum(((univ.party.age) / 3700) + 1);
	
	dlog.run();
	
	long long dialog_answer = minmax(0,500,dlog.getResult<long long>());
	
	univ.party.age = (long long) (3700) * (long long) (dialog_answer);
}

void edit_xp(cPlayer *pc) {
	location view_loc;
	
	set_cursor(sword_curs);
	
	cDialog dlog(*ResMgr::dialogs.get("edit-xp"));
	dlog["okay"].attachClickHandler(get_num_event_filter);
	
	dlog["number"].setTextToNum(pc->experience);
	dlog["perlevel"].setTextToNum(pc->get_tnl());
	
	dlog.run();
	
	int dialog_answer = minmax(0,10000,abs(dlog.getResult<long long>()));
	
	pc->experience = dialog_answer;
}


