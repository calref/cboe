
#include <SFML/Window/Event.hpp>
#include "dialog.hpp"

bool handle_action(const sf::Event&);
void flash_rect(rectangle to_flash);
void edit_gold_or_food(short which_to_edit);
// mode=0 : mage, mode=1 : priest
void display_pc(short pc_num,short mode,bool edit, cDialog* parent);
void display_alchemy(bool allowEdit,cDialog* parent);
bool spend_xp(short pc_num, short mode, cDialog* parent);
void edit_day();
void edit_xp(cPlayer *pc);
