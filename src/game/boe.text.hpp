#include <string>

class cVehicle;

void put_pc_screen();
void place_buy_button(short position,short pc_num,short item_num);
void put_item_screen(short screen_num);
void place_item_bottom_buttons();
void set_stat_window(short new_stat);
void place_item_button(short which_button_to_put,short which_slot,short which_button_position,short extra_val);
short first_active_pc();
void refresh_stat_areas(short mode);
short total_encumbrance(short pc_num);
void draw_pc_effects(short pc);
void print_party_stats() ;
short do_look(location space);
cVehicle* town_boat_there(location where);
cVehicle* out_boat_there(location where);
cVehicle* town_horse_there(location where);
cVehicle* out_horse_there(location where);
void notify_out_combat_began(cOutdoors::cWandering encounter,short *nums) ;
std::string get_m_name(mon_num_t num);
std::string get_ter_name(ter_num_t num);
void print_monst_name(mon_num_t m_type);
void damaged_message(short damage,eMonstMelee type);
std::string print_monster_going(mon_num_t m_num,short ap);
void print_nums(short a,short b,short c);
short print_terrain(location space);
void add_string_to_buf(std::string str, unsigned short indent); // Set second paramater to nonzero to auto-split the line if it's too long
void add_string_to_buf(std::string str);
void init_buf();
void print_buf () ;
void restart_printing();
void restore_mode();
void through_sending();
rectangle coord_to_rect(short i,short j);
bool day_reached(unsigned short which_day, unsigned short which_event);
void Draw_Some_Item (sf::Texture& src_gworld, rectangle src_rect, sf::RenderTarget& targ_gworld, location target, char masked, short main_win);
rectangle get_stat_effect_rect(int which_effect);

struct text_label_t {
	rectangle text_rect;
	std::string str;
};

void place_text_label(std::string string, location at, bool centred);
void draw_text_label(const text_label_t& label);
