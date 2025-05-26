
#include "dialogxml/dialogs/dialog.hpp"
#include "dialogxml/widgets/pict.hpp"

bool GTP(short item_num);
bool silent_GTP(short item_num);

void give_gold(short amount,bool print_result);
void give_food(short amount,bool print_result);
// Try to take gold, but take none and return false if the party can't afford it.
bool take_gold(short amount,bool print_result);
// Take food until the party has 0, returning the amount of their deficiency
short take_food(short amount,bool print_result);

void equip_item(short pc_num,short item_num);
void drop_item(short pc_num,short item_num,location where_drop);
bool place_item(cItem item,location where,bool try_contained = false);
void give_thing(short pc_num, short item_num);
short dist_from_party(location where);
void set_item_flag(cItem *item);
short get_item(location place,short pc_num,bool check_container);

void make_town_hostile();
void set_town_attitude(short lo,short hi,eAttitude att);
bool show_get_items(std::string titleText, std::vector<cItem*>& itemRefs, short pc_getting, bool overload = false);
bool display_item(location from_loc,short pc_num,short mode, bool check_container);
void custom_pic_dialog(std::string title, pic_num_t bigpic);
void story_dialog(std::string title, str_num_t first, str_num_t last, eSpecCtxType which_str_type, pic_num_t pic, ePicType pt, short anim_loops, int anim_fps);
short get_num_of_items(short max_num);
void init_mini_map();
void draw_help_dialog_item_buttons(cDialog& dialog,short item);
void draw_help_dialog_forcefields(cDialog& dialog,short item);
void place_glands(location where,mon_num_t m_type);
void reset_item_max();
short item_val(cItem item);
void place_treasure(location where,short level,short loot,short mode);
std::string get_text_response(std::string prompt = "", pic_num_t pic = 16);

enum class eSelectPC {
	ANY,
	ONLY_LIVING,
	ONLY_LIVING_WITH_ITEM_SLOT,
	// Slightly different from the previous: this now checks not just for a slot,
	// but also for carrying capacity, and will stack charges even if all slots are full.
	// When you use this, you have to set the global item_store to your item, first.
	ONLY_CAN_GIVE,
	// Same as the previous, but in combat, only show *adjacent* living PCs who can take the item.
	ONLY_CAN_GIVE_FROM_ACTIVE,
	// Must have lockpicks equipped
	ONLY_CAN_LOCKPICK,
	// Must have skill points
	ONLY_CAN_TRAIN,
	ONLY_DEAD,
	ONLY_STONE,
};
// Prompt the player to choose a party member. Returns 0-5 for a pc, 6 for cancel, 7 for all, or 8 if no PCs fit the mode's filter.
// Pass a string poiner to no_choice_reason to get the reason why no choices were available, if none are.
short select_pc(eSelectPC mode, std::string title="", eSkill highlight_highest = eSkill::INVALID, bool allow_choose_all = false);
