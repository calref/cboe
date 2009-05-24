#ifndef _ITEMS_H
	#define _ITEMS_H

#include "location.h"
#include "item.h"
#include "global.h"

Boolean give_to_party(item_record_type item,short print_result);
Boolean forced_give(short item_num,short abil);
Boolean party_take_abil(short abil);
short item_weight(item_record_type item);
Boolean place_item(item_record_type item,location where,Boolean forced);
void destroy_an_item();
void set_item_flag(item_record_type *item);
short get_item(location place,short pc_num,Boolean check_container);
void put_item_graphics();
void make_town_hostile();
void display_item_event_filter (short item_hit);
short display_item(location from_loc,short pc_num,short mode, bool check_container);
void fancy_choice_dialog_event_filter (short item_hit);
short fancy_choice_dialog(short which_dlog,short parent);
void select_pc_event_filter (short item_hit);
short char_select_pc(short active_only,short free_inv_only,char *title);
short select_pc(short active_only,short free_inv_only);
void get_num_of_items_event_filter (short item_hit);
short get_num_of_items(short max_num);
short choice_dialog(short pic,short num);
void make_cursor_watch() ;
void place_glands(location where,unsigned char m_type);
void reset_item_max();
short item_val(item_record_type item);
void place_treasure(location where,short level,short loot,short mode);
item_record_type return_treasure(short loot,short level,short mode);
void frame_button(RECT button_rect);
void refresh_store_items();
void get_text_response_event_filter (short item_hit);
void get_text_response(short dlg,char *str,short parent_num);
short custom_choice_dialog(char *strs,short pic_num,short buttons[3]) ;

#endif
