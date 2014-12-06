
#ifndef BOE_GAME_DLGUTIL_H

#include "dialog.h"
#include "simpletypes.h"

void start_shop_mode(short shop_type,short shop_min,short shop_max,short cost_adj,const char* store_name);
void end_shop_mode();
void handle_shop_event(location p);
void handle_sale(short what_chosen,short cost);
void handle_info_request(short what_chosen);
void set_up_shop_array();
void start_talk_mode(short m_num,short personality,m_num_t monst_type,short store_face_pic);
void end_talk_mode();
void handle_talk_event(location p);
void handle_talk_spec(short ttype,char* place_string1,char* place_string2);
void store_responses();
void do_sign(short town_num, short which_sign, short sign_type);
void do_talk(short personality, unsigned short m_num);
void put_party_stats();
void edit_party();
void pick_preferences()  ;
//void check_pref_file() ;
//long open_pref_file();
//void make_pref_file(FSSpec pref);
void save_prefs(bool resetHelp = false);
void load_prefs();
void tip_of_day();
short pick_a_scen();
short pick_prefab_scen();
//void give_password_filter (short item_hit);
//bool enter_password();
//short wd_to_pwd(Str255 str);

#endif
