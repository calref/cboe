
void init_scenario() __attribute__((deprecated));
short edit_ter_type(ter_num_t which_ter);
short edit_monst_type(short which_monst);
cMonster edit_monst_abil(cMonster starting_record,short parent_num);
short edit_item_type(short which_item);
cItemRec edit_item_abil(cItemRec starting_record,short parent_num);
void edit_spec_item(short which_item);
void edit_save_rects();
void edit_horses();
void edit_add_town();
void edit_item_placement();
void edit_scen_details();
short edit_make_scen_2(short *val_array);
short edit_make_scen_1(char *filename,char *title,short *grass);
void edit_scenario_events();
bool build_scenario();
void set_starting_loc();
void edit_boats();

