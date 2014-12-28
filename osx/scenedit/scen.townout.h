
void edit_placed_monst(short which_m);
cCreature edit_placed_monst_adv(cCreature monst_record, class cDialog& parent);
void edit_sign(short which_sign,short picture);
void edit_roomdescs(bool town);
short pick_town_num(std::string which_dlog,short def,cScenario& scenario);
bool change_ter(short& change_from,short& change_to,short& chance);
void edit_out_wand(short mode);
void outdoor_details();
void edit_town_details();
void edit_town_events();
void edit_advanced_town();
void edit_basic_dlog(short which_node);
void edit_talk_node(short which_node);
location pick_out(location default_loc);
short pick_import_town(short def,fs::path& temp_file_to_load);
bool new_town(short which_town);
void edit_placed_item(short which_i);

void delete_last_town();
void edit_town_wand();
