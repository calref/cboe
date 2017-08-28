
#include <SFML/Graphics.hpp>

int main(int argc, char* argv[]);
void Handle_One_Event();
void Handle_Update();
void Mouse_Pressed();
void close_program();
void change_cursor(location where_curs);
void set_up_apple_events(int argc, char* argv[]);
void move_sound(ter_num_t ter,short step);
void incidental_noises(bool on_surface);
void pause(short length);
bool handle_startup_press(location the_point);
void handle_splash_events();
void show_logo();
void plop_fancy_startup();
