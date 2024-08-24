
#ifndef BOE_GAME_ACTIONS_H
#define BOE_GAME_ACTIONS_H

#include <SFML/Window/Event.hpp>
#include "location.hpp"
#include "dialogxml/keycodes.hpp"
#include "tools/framerate_limiter.hpp"

void init_screen_locs();
bool prime_time();
bool handle_action(const sf::Event& event, cFramerateLimiter& fps_limiter);
void advance_time(bool did_something, bool need_redraw, bool need_reprint);
void handle_move(location destination, bool& did_something, bool& need_redraw, bool& need_reprint);
void handle_monster_actions(bool& need_redraw, bool& need_reprint);
bool someone_awake();
void handle_menu_spell(short spell_picked,short spell_type) ;
void initiate_outdoor_combat(short i);
bool handle_keystroke(const sf::Event& event, cFramerateLimiter& fps_limiter);
bool handle_scroll(const sf::Event& event);
void do_load();
void post_load();
void do_save(short mode);
void do_abort();
void increase_age();
void handle_hunting();
void switch_pc(short which);
void handle_drop_pc();
void new_party();
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

void handle_startup_button_click(eStartButton btn, eKeyMod mods);
void handle_switch_pc(short which_pc, bool& need_redraw, bool& need_reprint);
void handle_switch_pc_items(short which_pc, bool& need_redraw);
void handle_equip_item(short item_hit, bool& need_redraw);
void handle_use_item(short item_hit, bool& did_something, bool& need_redraw);
void handle_item_shop_action(short item_hit);
void handle_alchemy(bool& need_redraw, bool& need_reprint);
void handle_town_wait(bool& need_redraw, bool& need_reprint);
void handle_combat_switch(bool& did_something, bool& need_redraw, bool& need_reprint);
void handle_missile(bool& need_redraw, bool& need_reprint);
void handle_get_items(bool& did_something, bool& need_redraw, bool& need_reprint);
void handle_drop_item(short item_hit, bool& need_redraw);
void handle_drop_item(location destination, bool& need_redraw);
void handle_give_item(short item_hit, bool& did_something, bool& need_redraw);
void show_dialog_action(std::string xml_file);
void handle_new_pc_graphic();
void handle_rename_pc();

#endif
