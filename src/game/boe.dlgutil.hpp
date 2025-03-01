
#ifndef BOE_GAME_DLGUTIL_H

#include <string>
#include "dialogxml/dialogs/dialog.hpp"
#include "scenario/shop.hpp"

bool start_shop_mode(short which,short cost_adj,std::string store_name, bool cancel_when_empty = false);
void end_shop_mode();
bool handle_shop_event(location p, cFramerateLimiter& fps_limiter);
void handle_sale(int i);
void handle_info_request(int what_picked);
void set_up_shop_array();
void start_talk_mode(short m_num,short personality,mon_num_t monst_type,short store_face_pic);
void end_talk_mode();
void handle_talk_node(int which_talk_entry);
bool handle_talk_event(location p, cFramerateLimiter& fps_limiter);
void handle_talk_spec(short ttype,char* place_string1,char* place_string2);
void store_responses();
void do_sign(short town_num, short which_sign, short sign_type);
void do_talk(short personality, unsigned short m_num);
void put_party_stats();
void edit_party();
void pick_preferences(bool record = true);
void save_prefs();
void tip_of_day();
struct scen_header_type pick_a_scen();
fs::path run_file_picker(bool saving);
// Pick from the autosaves made while playing in a given save file
fs::path run_autosave_picker(fs::path save_file);

#endif
