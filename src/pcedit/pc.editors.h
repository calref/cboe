

bool give_to_pc(short pc_num,cItem item, short print_result);
bool give_to_party(cItem item,short print_result);
void give_gold(short amount,bool print_result);
bool take_gold(short amount,bool print_result);
short pc_has_space(short pc_num);
void take_item(short pc_num,short which_item);
short char_select_pc(short active_only,short free_inv_only,const char *title);
short select_pc(short active_only,short free_inv_only);
void give_spec_items();
void pick_race_abil(cPlayer *pc,short mode);
void reset_boats();
void combine_things(short pc_num);
