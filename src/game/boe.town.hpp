
#include <SFML/Graphics.hpp>
#include "scenario/outdoors.hpp"

void force_town_enter(short which_town,location where_start);
void start_town_mode(short which_town, short entry_dir, bool debug_enter = false);
location end_town_mode(bool switching_level,location destination,bool debug_leave=false);  // returns new party location
void handle_leave_town_specials(short town_number, short which_spec,location start_loc) ;
void handle_town_specials(short town_number, bool town_dead,location start_loc) ;
bool abil_exists(eItemAbil abil);

void start_town_combat(eDirection direction);
eDirection end_town_combat();
void place_party(short direction);
void create_out_combat_terrain(short type,short num_walls,bool is_road);
void elim_monst(ter_num_t which,short spec_a,short spec_b);
void do_shop(short which,short min,short max,char *store_name);
void buy_food(short cost,short per,const char* food_name);
void healing_shop();
void do_sell(short which);
void dump_gold(short print_mes);
bool is_unlockable(location where);
void pick_lock(location where,short pc_num);
void bash_door(location where,short pc_num);
void erase_town_specials();
void erase_hidden_towns(cOutdoors& sector, int quadrant_x, int quadrant_y);
void erase_completed_specials(cArea& sector, std::function<void(location)> clear_spot);
void erase_out_specials();
bool does_location_have_special(cOutdoors& sector, location loc, eTerSpec type);
void clear_map();
rectangle minimap_view_rect();
void draw_map(bool need_refresh, std::string tooltip_text = "");
bool is_door(location destination);
void display_map();
void check_done();
bool quadrant_legal(short i, short j) ;

enum ePushableThing {
	PUSH_CRATE,
	PUSH_BARREL,
	PUSH_BLOCK,
};
void push_thing(ePushableThing type, location pusher_loc, location thing_loc);
void move_thing(ePushableThing type, location from_loc, location to_loc);
