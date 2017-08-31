
#include "dialog.hpp"
#include "pict.hpp"

bool GTP(short item_num);
bool silent_GTP(short item_num);
void give_gold(short amount,bool print_result);
bool take_gold(short amount,bool print_result);
void give_food(short amount,bool print_result);
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
short custom_choice_dialog(std::array<std::string, 6>& strs,short pic_num,ePicType pic_type,std::array<short, 3>& buttons) ;
void custom_pic_dialog(std::string title, pic_num_t bigpic);
void story_dialog(std::string title, str_num_t first, str_num_t last, int which_str_type, pic_num_t pic, ePicType pt);
short get_num_of_items(short max_num);
void init_mini_map();
void draw_help_dialog_item_buttons(cDialog& dialog,short item);
void draw_help_dialog_forcefields(cDialog& dialog,short item);
void place_glands(location where,mon_num_t m_type);
void reset_item_max();
short item_val(cItem item);
void place_treasure(location where,short level,short loot,short mode);
std::string get_text_response(std::string prompt = "", pic_num_t pic = 16);
short get_num_response(short min, short max, std::string prompt);

short char_select_pc(short mode,const char *title);
short select_pc(short mode);
