
class cDialog;

void edit_custom_sounds();
void edit_custom_sheets();
void edit_custom_pics_types();
bool edit_ter_type(ter_num_t which_ter);
bool edit_monst_type(short which_monst);
cMonster edit_monst_abil(cMonster starting_record,short which_monst,cDialog& parent);
bool edit_item_type(short which_item);
cItem edit_item_abil(cItem starting_record,short which_item,cDialog& parent);
bool edit_spec_item(short which_item);
bool edit_quest(size_t which_quest);
bool edit_shop(size_t which_shop, cDialog* parent = nullptr);
void edit_save_rects();
void edit_add_town();
void edit_item_placement();
void edit_scen_details();
void edit_scen_adv_details();
bool edit_make_scen_2(short& out_w, short& out_h, short& town_l, short& town_m, short& town_s, bool& def_town);
bool edit_make_scen_1(std::string& filename,std::string& title,bool& grass);
void edit_scenario_events();
bool build_scenario();
bool edit_vehicle(class cVehicle& what, int num, bool is_boat);
class cArea* get_current_area();

bool check_range_msg(cDialog& me,std::string id,bool losing,long min_val,long max_val,std::string fld_name,std::string xtra);
bool check_range(cDialog& me,std::string id,bool losing,long min_val,long max_val,std::string fld_name);
bool pick_string(std::string from_file, cDialog& parent, std::string result_fld, std::string str_fld);
