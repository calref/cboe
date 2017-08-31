
#ifndef BOE_GAME_ACTIONS_H
#define BOE_GAME_ACTIONS_H

#include <SFML/Window/Event.hpp>
#include "location.hpp"

void init_screen_locs();
bool prime_time();
bool handle_action(sf::Event event);
void handle_monster_actions(bool& need_redraw, bool& need_reprint);
bool someone_awake();
void handle_menu_spell(short spell_picked,short spell_type) ;
void initiate_outdoor_combat(short i);
bool handle_keystroke(sf::Event& event);
bool handle_scroll(sf::Event& event);
void do_load();
void post_load();
void do_save(short mode);
void increase_age();
void handle_hunting();
void switch_pc(short which);
void drop_pc(short which);
void handle_death();
void start_new_game(bool force = false);
location get_cur_direction(location the_point);
bool outd_move_party(location destination,bool forced);
bool town_move_party(location destination,short forced);
bool someone_poisoned();
short nearest_monster();
void setup_outdoors(location where);
short get_outdoor_num();
short count_walls(location loc);
bool is_sign(ter_num_t ter);
bool check_for_interrupt();

#endif
