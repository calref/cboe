#include <SFML/Graphics.hpp>

void init_main_buttons();
void Set_up_win ();
void redraw_screen();
void do_button_action(short which_pc,short which_button);
sf::Vector2f translate_mouse_coordinates(sf::Vector2i const point);
sf::RenderWindow& mainPtr();
