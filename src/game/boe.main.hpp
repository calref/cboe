
#include <SFML/Graphics.hpp>

#ifdef __APPLE__
extern eMenuChoice menuChoice;
extern short menuChoiceId;
#endif
int main(int argc, char* argv[]);
void update_everything();
void redraw_everything();
void Mouse_Pressed(const sf::Event&);
void close_program();
void change_cursor(location where_curs);
void set_up_apple_events();
void move_sound(ter_num_t ter,short step);
void incidental_noises(bool on_surface);
void pause(short length);
bool handle_startup_press(location the_point);
void handle_splash_events();
void show_logo();
void plop_fancy_startup();
void update_terrain_animation();
void update_startup_animation();
void handle_events();
void handle_one_event(const sf::Event&);
void handle_one_minimap_event(const sf::Event &);

