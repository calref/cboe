
void edit_placed_monst(short which_m);
cTownperson edit_placed_monst_adv(cTownperson monst_record, short which, class cDialog& parent);
void edit_sign(short which_sign,short picture);
short pick_town_num(std::string which_dlog,short def,cScenario& scenario);
bool change_ter(short& change_from,short& change_to,short& chance);
void edit_out_wand(short mode);
void outdoor_details();
void edit_town_details();
void edit_town_events();
void edit_advanced_town();
void edit_basic_dlog(short which_node);
short edit_talk_node(short which_node);
location pick_out(location default_loc);
cTown* pick_import_town(short def);
bool new_town(short which_town);
void edit_placed_item(short which_i);

void delete_last_town();
void edit_town_wand();
