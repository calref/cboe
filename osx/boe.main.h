
#include <SFML/Graphics.hpp>

int main(int argc, char* argv[]);
void Initialize(void);
void Handle_One_Event();
void Handle_Update();
void Mouse_Pressed();
void close_program();
//void handle_menu_choice(long choice);
void handle_apple_menu(int item_hit);
void handle_file_menu(int item_hit);
void handle_options_menu(int item_hit);
void handle_help_menu(int item_hit);
void handle_library_menu(int item_hit);
void handle_actions_menu(int item_hit);
void handle_mage_spells_menu(int item_hit);
void handle_priest_spells_menu(int item_hit);
void handle_monster_info_menu(int item_hit);
void handle_music_menu(int item_hit);
void change_cursor(location where_curs);
void set_up_apple_events();
void move_sound(ter_num_t ter,short step);
void incidental_noises(bool on_surface);
void pause(short length);
bool sd_legit(short a, short b);
