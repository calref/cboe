void init_directories();
void save_scenario();
void load_scenario();
void augment_terrain(location to_create);
void load_outdoors(location which_out,short mode);
void load_town(short which_town);
void import_town(short which_town);
void create_basic_scenario();
void make_new_scenario(Str255 file_name,short out_width,short out_height,short making_warriors_grove,
	short use_grass);

void oops_error(short error);
short init_data(short flag);
short town_s(short flag);
short out_s(short flag);
short str_size_1(short flag);
short str_size_2(short flag);
short str_size_3(short flag);
short get_buf_ptr(short flag);
Boolean check_p (short pword);
Boolean check_p2 (short pword);
Boolean check_p3 (short pword);
void reset_pwd();
void start_data_dump();
Boolean import_check_p (short pword);
void scen_text_dump();

void port_talk_nodes();
void port_town();
void port_t_d();
void port_scenario();
void port_item_list();
void port_out(outdoor_record_type *out);
void flip_spec_node(special_node_type *spec);
void flip_short(short *s);
void flip_rect(Rect *s);
void port_dummy_talk_nodes();
void port_dummy_t_d(short size,char *buffer);
void port_dummy_town();
void load_spec_graphics();

