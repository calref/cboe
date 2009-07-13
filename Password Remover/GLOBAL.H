#ifndef _GLOBAL_H
	#define _GLOBAL_H

#define FALSE 0
#define TRUE 1

typedef char Boolean;

typedef struct tagRECT { 
   long left;
   long top;
   long right;
   long bottom;
} RECT;

struct RECT16
{
	short left;
	short top;
	short right;
	short bottom;
	
	RECT rect32();
	RECT16 () {}
	RECT16 (const RECT & );
	RECT16 & operator=(const RECT &);
};

typedef struct {
	char x,y;
	} location;

typedef struct {
	short picture;
	unsigned char blockage,flag1,flag2,special,trans_to_what,fly_over,boat_over;
	unsigned char block_horse,light_radius,step_sound,shortcut_key,res1,res2,res3;
	} terrain_type_type;
	
typedef struct {
	unsigned char m_num,level,m_name[26];
	short health,m_health,mp,max_mp;
	unsigned char armor,skill;
	short a[3];
	unsigned char a1_type,a23_type,m_type,speed,ap,mu,cl,breath,breath_type,treasure,spec_skill,poison;
	short morale,m_morale;
	short corpse_item,corpse_item_chance;
	short status[15];
	unsigned char direction,immunities,x_width,y_width,radiate_1,radiate_2;
	unsigned char default_attitude,summon_type,default_facial_pic,res1,res2,res3;
	short picture_num;
	} monster_record_type;	
	
typedef struct {
	short ter_type,item_num[10],item_odds[10],property;
	} item_storage_shortcut_type;
	
typedef struct {
	location horse_loc,horse_loc_in_sec,horse_sector;
	short which_town;
	Boolean exists,property;
	} horse_record_type;
	
typedef struct {
	location boat_loc,boat_loc_in_sec,boat_sector;
	short which_town;
	Boolean exists,property;
} boat_record_type;

typedef struct {
	short type,sd1,sd2,pic,m1,m2,ex1a,ex1b,ex2a,ex2b,jumpto;
	} special_node_type;


	typedef struct {
	unsigned char flag1, flag2, flag3, flag4;
	unsigned char ver[3],min_run_ver,prog_make_ver[3],num_towns;
	unsigned char out_width,out_height,difficulty,intro_pic,default_ground;
	unsigned char town_size[200];
	unsigned char town_hidden[200],a;
	short flag_a;
	short intro_mess_pic,intro_mess_len;
	location where_start,out_sec_start,out_start;
	short which_town_start;
	short flag_b;
	short town_data_size[200][5];
	short town_to_add_to[10];
	short flag_to_add_to_town[10][2];
	short flag_c;
	short out_data_size[100][2];
	RECT16 store_item_rects[3];
	short store_item_towns[3];
	short flag_e;
	short special_items[50];
	short special_item_special[50];
	short rating,uses_custom_graphics;
	short flag_f;
	monster_record_type scen_monsters[256];
	boat_record_type scen_boats[30];
	horse_record_type scen_horses[30];
	short flag_g;
	terrain_type_type ter_types[256];
	short scenario_timer_times[20];
	short scenario_timer_specs[20];
	short flag_h;
	special_node_type scen_specials[256];
	item_storage_shortcut_type storage_shortcuts[10];
	short flag_d;
	unsigned char scen_str_len[300];
	short flag_i;
	location last_out_edited;
	short last_town_edited;

	} scenario_data_type;
	
short get_ran (short times,short  min,short  max);
short s_pow(short x,short y);
#endif
