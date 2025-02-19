
#ifndef BOE_GAME_ACTIONS_H
#define BOE_GAME_ACTIONS_H

#include <vector>
#include <SFML/Window/Event.hpp>
#include "location.hpp"
#include "dialogxml/keycodes.hpp"
#include "tools/framerate_limiter.hpp"

struct key_action_t {
    std::vector<char> keys;
    std::string name;
    void (*action)();
};

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
void do_save(bool save_as = false);
void do_abort();
void increase_age();
void handle_hunting();
void switch_pc(short which);
void handle_drop_pc();
void handle_new_pc();
void new_party();
void handle_death();
void start_new_game(bool force = false);
void start_tutorial();
location get_cur_direction(location the_point);
bool outd_move_party(location destination,bool forced);
bool town_move_party(location destination,short forced);
bool someone_poisoned();
short nearest_monster();
void setup_outdoors(location where);
short get_outdoor_num();
short count_walls(location loc);
bool is_sign(ter_num_t ter);
bool check_for_interrupt(std::string confirm_dialog = "confirm-interrupt-special");

void handle_startup_button_click(eStartButton btn, eKeyMod mods);
void handle_switch_pc(short which_pc, bool& need_redraw, bool& need_reprint);
void handle_switch_pc_items(short which_pc, bool& need_redraw);
void handle_equip_item(short item_hit, bool& need_redraw);
void handle_use_item(short item_hit, bool& did_something, bool& need_redraw);
void handle_item_shop_action(short item_hit);
void handle_alchemy(bool& need_redraw, bool& need_reprint);
void handle_wait(bool& did_something, bool& need_redraw, bool& need_reprint);
void handle_combat_switch(bool& did_something, bool& need_redraw, bool& need_reprint);
void handle_missile(bool& need_redraw, bool& need_reprint);
void handle_get_items(bool& did_something, bool& need_redraw, bool& need_reprint);
void handle_drop_item(short item_hit, bool& need_redraw);
void handle_drop_item(location destination, bool& need_redraw);
void handle_give_item(short item_hit, bool& did_something, bool& need_redraw);
void handle_toggle_active(bool& need_reprint);
void handle_parry(bool& did_something, bool& need_redraw, bool& need_reprint);
void show_dialog_action(std::string xml_file);
void handle_new_pc_graphic();
void handle_rename_pc();
void handle_begin_look(bool right_button, bool& need_redraw, bool& need_reprint);
void handle_look(location destination, bool right_button, eKeyMod mods, bool& need_redraw, bool& need_reprint);
void screen_shift(int dx, int dy, bool& need_redraw);
bool handle_screen_shift(location delta, bool& need_redraw);
void handle_rest(bool& need_redraw, bool& need_reprint);
void handle_spellcast(eSkill which_type, bool& did_something, bool& need_redraw, bool& need_reprint, bool record = true);
void handle_target_space(location destination, bool& did_something, bool& need_redraw, bool& need_reprint);
void handle_pause(bool& did_something, bool& need_redraw);
void handle_bash_pick_select(bool& need_reprint, bool isBash);
void handle_bash_pick(location destination, bool& did_something, bool& need_redraw, bool isBash);
void handle_use_space_select(bool& need_reprint);
void handle_use_space(location destination, bool& did_something, bool& need_redraw);
void show_inventory();
void toggle_debug_mode();
void init_debug_actions();
void show_debug_help();
void debug_fight_encounter(bool wandering);
void debug_give_item();
void debug_overburden();
void debug_print_location();
void debug_step_through();
void debug_leave_town();
void debug_kill();
void debug_kill_party();
void debug_magic_map();
void debug_enter_town();
void debug_refresh_stores();
void debug_clean_up();
void debug_stealth_detect_life_firewalk();
void debug_fly();
void debug_ghost_mode();
void debug_return_to_start();
void handle_victory(bool force = false, bool record = false);
void debug_increase_age();
void debug_towns_forget();
void debug_heal_plus_extra();
void debug_heal();
void handle_print_pc_hp(int which_pc, bool& need_reprint);
void handle_print_pc_sp(int which_pc, bool& need_reprint);
void handle_trade_places(int which_pc, bool& need_reprint);
void handle_begin_talk(bool& need_reprint);
void handle_talk(location destination, bool& did_something, bool& need_redraw, bool& need_reprint);
void give_help_and_record(short help1, short help2, bool help_forced = false);
void show_item_info(short item_hit);
void close_map(bool record = false);
void cancel_item_target(bool& did_something, bool& need_redraw, bool& need_reprint);
void update_item_stats_area(bool& need_reprint);
void easter_egg(int idx);
void preview_dialog_xml();
void preview_every_dialog_xml();

#endif
