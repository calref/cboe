#ifndef _ACTIONS_H
	#define _ACTIONS_H

void init_screen_locs();
Boolean prime_time();
Boolean handle_action(POINT the_point, WPARAM wparam, LPARAM lparam );
Boolean someone_awake();
void flash_rect(RECT to_flash);
void flash_round_rect(RECT to_flash,short radius);
void button_flash_rect(RECT to_flash);
void handle_menu_spell(short spell_picked,short spell_type) ;
void initiate_outdoor_combat(short i);
void initiate_redraw();
Boolean handle_syskeystroke(WPARAM wParam,LPARAM lParam,short *handled);
Boolean handle_keystroke(WPARAM wParam,LPARAM lParam);
void menu_activate( short type);
void do_load();
void post_load();
void do_save(short mode);
void increase_age();
void switch_pc(short which);
void handle_death();
void start_new_game();
location get_cur_direction(POINT the_point);
Boolean outd_move_party(location destination,Boolean forced);
Boolean town_move_party(location destination,short forced);
Boolean someone_poisoned();
short nearest_monster();
void setup_outdoors(location where);
short get_outdoor_num();
Boolean is_sign(unsigned char ter);
void check_cd_event(HWND hwnd,UINT message,WPARAM wparam,LPARAM lparam);
void set_up_lights();
#endif
