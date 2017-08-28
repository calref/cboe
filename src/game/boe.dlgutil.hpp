
#ifndef BOE_GAME_DLGUTIL_H

#include <string>
#include "dialog.hpp"
#include "shop.hpp"

void start_shop_mode(short which,short cost_adj,std::string store_name);
void end_shop_mode();
void handle_shop_event(location p);
void handle_sale(cShopItem item, int i);
void handle_info_request(cShopItem item);
void set_up_shop_array();
void start_talk_mode(short m_num,short personality,mon_num_t monst_type,short store_face_pic);
void end_talk_mode();
void handle_talk_event(location p);
void handle_talk_spec(short ttype,char* place_string1,char* place_string2);
void store_responses();
void do_sign(short town_num, short which_sign, short sign_type);
void do_talk(short personality, unsigned short m_num);
void put_party_stats();
void edit_party();
void pick_preferences();
void save_prefs(bool resetHelp = false);
void tip_of_day();
short pick_a_scen();
short pick_prefab_scen();

#endif
