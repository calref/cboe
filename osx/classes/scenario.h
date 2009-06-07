/*
 *  scenario.h
 *  BoE
 *
 *  Created by Celtic Minstrel on 20/04/09.
 *
 */

#ifndef SCENARIO_H
#define SCENARIO_H

#include <iosfwd>

namespace legacy{
	struct scenario_data_type;
	struct item_storage_shortcut_type;
	struct scen_item_data_type;
};

struct scenario_header_flags {
	unsigned char flag1, flag2, flag3, flag4;
	unsigned char ver[3],min_run_ver,prog_make_ver[3],num_towns;
};

class cScenario {
public:
	class cItemStorage {
	public:
		short ter_type;
		short item_num[10];
		short item_odds[10];
		short property;
		cItemStorage();
		cItemStorage& operator = (legacy::item_storage_shortcut_type& old);
	};
public:
	//unsigned char flag1, flag2, flag3, flag4;
	unsigned char num_towns;
	unsigned char out_width,out_height,difficulty,intro_pic,default_ground;
	unsigned char town_size[200];
	unsigned char town_hidden[200];
	short flag_a; // TODO: Remove these flags
	short intro_mess_pic,intro_mess_len;
	location where_start,out_sec_start,out_start;
	short which_town_start;
	short flag_b;
	short town_data_size[200][5];
	short town_to_add_to[10];
	short flag_to_add_to_town[10][2];
	short flag_c;
	short out_data_size[100][2];
	rectangle store_item_rects[3];
	short store_item_towns[3];
	short flag_e;
	short special_items[50];
	short special_item_special[50];
	short rating,uses_custom_graphics;
	short flag_f;
	cMonster scen_monsters[256];
	cVehicle boats[30];
	cVehicle horses[30];
	short flag_g;
	cTerrain ter_types[256];
	short scenario_timer_times[20];
	short scenario_timer_specs[20];
	short flag_h;
	cSpecial scen_specials[256];
	cItemStorage storage_shortcuts[10];
	short flag_d;
	unsigned char scen_str_len[300];
	short flag_i;
	location last_out_edited;
	short last_town_edited;
	scenario_header_flags format;
	// scen_item_data_type scen_item_list {
	cItemRec scen_items[400];
	//char monst_names[256][20];
	//char ter_names[256][30];
	// };
	//char scen_strs[270][256];
	char scen_name[256];
	char who_wrote[2][256];
	char contact_info[256];
	char intro_strs[6][256];
	char journal_strs[50][256];
	//char spec_item_strs[100][256];
	char spec_item_names[50][256];
	char spec_item_strs[50][256];
	char spec_strs[100][256];
	char monst_strs[100][256];
	FSSpec scen_file; // transient
	cOutdoors* outdoors;
	cTown* towns;
	
	char(& scen_strs(short i))[256] __attribute__((deprecated));
	cScenario& operator = (legacy::scenario_data_type& old);
	void append(legacy::scen_item_data_type& old);
	void writeTo(std::ostream& file);
};

#endif