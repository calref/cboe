#ifndef _GLOBAL_H
	#define _GLOBAL_H

#include <windows.h>
#include <cstdio>		/* for sprintf */
#include <cstdlib>
#include <cmath>

#include "classes/consts.h"
#include "classes/item.h"
#include "classes/location.h"
#include "classes/pc.h"

#define huge



/*GK+*/
typedef unsigned char BYTE;

POINT MAKEPOINT(LONG lparam);
extern HINSTANCE store_hInstance;
void DebugQuit(char * msg);

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

#define T_M 	60

#define NUM_TOWN_ITEMS	115

#define	BITMAP_WIDTH	28
#define	BITMAP_HEIGHT	36

#define	PC_WIN_UL_X	291
#define	PC_WIN_UL_Y	5
#define	ITEM_WIN_UL_X	291
#define	ITEM_WIN_UL_Y	130
#define	TEXT_WIN_UL_X	291
#define	TEXT_WIN_UL_Y	283
#define	NUM_BUTTONS		15
#define ASB	add_string_to_buf
#define	PSD	party.stuff_done
#define	FCD	fancy_choice_dialog
#define	NUM_MONST_G	173

typedef char Boolean;

struct shortloc { short x, y; };

struct special_node_type
{
	short type,sd1,sd2,pic,m1,m2,ex1a,ex1b,ex2a,ex2b,jumpto;

	void flip();	/* reverse byte order */
};

typedef struct {
	short personality,type;
	char link1[4],link2[4];
	short extras[4];
	} talking_node_type;

typedef struct {
	unsigned char strlens[200];
	talking_node_type talk_nodes[60];
	} talking_record_type;

typedef struct {
	short picture;
	unsigned char blockage,flag1,flag2,special,trans_to_what,fly_over,boat_over;
	unsigned char block_horse,light_radius,step_sound,shortcut_key,res1,res2,res3;
	} terrain_type_type;

struct wandering_type
{
	unsigned char monst[4];

	bool isNull() const;
};

struct out_wandering_type
{
	unsigned char monst[7];
	unsigned char friendly[3];
	short spec_on_meet,spec_on_win,spec_on_flee,cant_flee;
	short end_spec1,end_spec2;

	bool isNull() const;
};

struct outdoor_record_type
{
	unsigned char	terrain[48][48];
	location	special_locs[18];
	unsigned char		special_id[18];
	location	exit_locs[8];
	char		exit_dests[8];
	location	sign_locs[8];
	out_wandering_type	wandering[4],special_enc[4];
	location	wandering_locs[4];
	RECT16 info_rect[8];
	unsigned char strlens[180];
	special_node_type specials[60];

	void flip();	/* reverse byte order */
};

typedef struct {
	unsigned char number;
	unsigned char start_attitude;
	location start_loc;
	unsigned char  mobile;
	unsigned char  time_flag;
	unsigned char  extra1,extra2;
	short spec1, spec2;
	char spec_enc_code, time_code;
	short monster_time,personality;
	short special_on_kill,facial_pic;
	} creature_start_type;

typedef struct {
	location item_loc;
	short item_code,ability;
	unsigned char charges,always_there,property,contained;
	} preset_item_type;

typedef struct {
	location field_loc;
	short field_type;
	} preset_field_type;

struct town_record_type
{
	short town_chop_time,town_chop_key;
	wandering_type	wandering[4];
	location	wandering_locs[4];
	location	special_locs[50];
	unsigned char		spec_id[50];
	location	sign_locs[15];
	short		lighting;
	location start_locs[4];
	location exit_locs[4];
	short exit_specs[4];
	RECT16 in_town_rect;
	preset_item_type preset_items[64];
	short max_num_monst;
	preset_field_type preset_fields[50];
	short spec_on_entry,spec_on_entry_if_dead;
	short timer_spec_times[8];
	short timer_specs[8];
	unsigned char strlens[180];
	special_node_type specials[100];
	unsigned char defy_mapping,defy_scrying;
    char hostile_spec_to_call,res2;
/*    bool strong_barriers : 1;
    bool defy_mapping : 1;
    char reserved_bits : 6;*/
	short difficulty;

	/* functions */
	location getSpecLoc(int spec);
};

struct big_tr_type
{
	unsigned char terrain[64][64];
	RECT16 room_rect[16];
	creature_start_type creatures[60];
	unsigned char lighting[8][64];
};

typedef struct {
	unsigned char terrain[48][48];
	RECT16 room_rect[16];
	creature_start_type creatures[40];
	unsigned char lighting[6][48];
	} ave_tr_type;

typedef struct {
	unsigned char terrain[32][32];
	RECT16 room_rect[16];
	creature_start_type creatures[30];
	unsigned char lighting[4][32];
	} tiny_tr_type;

typedef struct {
	item_record_type scen_items[400];
	char monst_names[256][20];
	char ter_names[256][30];
	} scen_item_data_type;

typedef struct {
	short ter_type,item_num[10],item_odds[10],property;
	} item_storage_shortcut_type;

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

/* CREATURE_DATA_TYPE */
class creature_data_type
{
	/* variables */
public:
	short active,attitude;
	unsigned char number;
	location m_loc;
	monster_record_type m_d;
	Boolean mobile;
	short summoned;
	creature_start_type monst_start;

	/* functions */
private:
	void adjustMagic(short *how_much);
public:
	void poison(short how_much);
	void acid(short how_much);
	void slow(short how_much);
	void curse(short how_much);
	void web(short how_much);
	void scare(short how_much);
	void disease(short how_much);
	void dumbfound(short how_much);
	void charm(short penalty, short which_status, short amount);
	void record();
};

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
	unsigned char flag1, flag2, flag3, flag4;
	unsigned char ver[3],min_run_ver,prog_make_ver[3],num_towns;
	unsigned char out_width,out_height,difficulty,intro_pic,default_ground;
	} scen_header_type;

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
//	short flag_i;
    short adjust_diff;
	location last_out_edited;
	short last_town_edited;
	} scenario_data_type;

// for game
typedef struct {
	short personality;
	short town_num;
	short str1,str2;
	} talk_save_type;

typedef struct {
	creature_data_type dudes[60];
	short which_town;
	short friendly;
	} creature_list_type;

struct current_town_type
{
	short town_num, difficulty;
	town_record_type	town;
	char explored[64][64];
	Boolean	hostile;
	creature_list_type	monst;
	Boolean	in_boat;
	location p_loc;

	/* functions */
	short placeMonster(unsigned char which, location where);
	short countMonsters();
	void activateMonsters(short code);
};

struct outdoor_creature_type
{
	Boolean exists;
	short direction;
	out_wandering_type what_monst;
	location which_sector,m_loc;
};

struct party_record_type
{
	long age;
	unsigned short gold,food;
	unsigned char stuff_done[310][10],item_taken[200][8]; //stuff_done[309] is compatibility switches
	short light_level;
	location outdoor_corner,i_w_c,p_loc,loc_in_sec;
	boat_record_type boats[30];
	horse_record_type horses[30];
	creature_list_type creature_save[4];
	short in_boat,in_horse;
	outdoor_creature_type out_c[10];
	item_record_type magic_store_items[5][10];
	short imprisoned_monst[4];
	char m_seen[256];
	char journal_str[50];
	short journal_day[50];
	short special_notes_str[140][2];
	talk_save_type talk_save[120];
	short direction,at_which_save_slot;
	char alchemy[20];
	Boolean can_find_town[200];
	short key_times[100];
	short party_event_timers[30];
	short global_or_town[30];
	short node_to_call[30];
	char spec_items[50],help_received[120];
	short m_killed[200];
	unsigned long total_m_killed,total_dam_done,total_xp_gained,total_dam_taken;
	char scen_name[256];

	/* functions */
	bool isFlying() { return (bool) stuff_done[305][1]; }

	void giveGold(short amount, bool print_result);
	bool takeGold(short amount, bool print_result);
	void giveFood(short amount, bool print_result);
	short takeFood(short amount, bool print_result);
};

typedef struct { char town_maps[200][8][64]; } stored_town_maps_type;
typedef struct { char town_strs[180][256]; } piles_of_stuff_dumping_type;

typedef struct {
	char scen_header_strs[3][80];
	char scen_names[256];
	} piles_of_stuff_dumping_type2;

typedef struct { char talk_strs[170][256]; } piles_of_stuff_dumping_type3;
typedef struct { char out_strs[9][256]; } outdoor_strs_type;
typedef struct { outdoor_strs_type outdoor_text[2][2]; } piles_of_stuff_dumping_type4;
typedef struct { char scen_strs[160][256]; } piles_of_stuff_dumping_type5;

class pc_array
{
private:
	//pc_record_type pc[6];
	pc_record_type pc[NUM_OF_PCS];

public:
	pc_record_type & operator[](int num) { return pc[num]; }

	/* remember - all this functions refer to all PCs */
	void affect(short type, short how_much);
	void cure(short how_much);							/* cure all */
	void disease(short how_much);						/* disease all */
	void dumbfound(short how_much);						/* dumb all */
	void damage(short how_much, short damage_type);		/* damage all */
	void heal(short how_much);							/* heal all */
	void kill(short mode);								/* kill all */
	void poison(short how_much);						/* poison all */

	void drainXP(short how_much);		// drain experience points

	void giveXP(short how_much);		// give experience points

	void restoreSP(short how_much);		// restore spell points

	bool hasAbil(short ability);

	bool isPoisoned();									/* is someone posioned? */

	short getMageLore();								/* count total mage lore */
	short getTotalLevel();
	short getTotalLuck();

	bool checkClass(short item_class,short mode);
};

typedef struct { unsigned char setup[4][64][64]; } setup_save_type;
typedef struct { unsigned char pattern[9][9]; } effect_pat_type;
typedef struct { item_record_type items[NUM_TOWN_ITEMS]; } stored_items_list;
typedef struct { char outdoor_maps[100][6][48]; } stored_outdoor_maps_type;

//extern short s_pow(short x,short y);
//extern short get_ran (short times, short min, short max);
Boolean same_point(location p1,location p2);
void pause(short length);
void Delay(short val,long *dummy);
void alter_rect(RECT *r) ;
void alter_rect(RECT16 *r) ;
Boolean sd_legit(short a, short b);

typedef  struct {

    short queued_special;
    long trigger_time;
    unsigned short mode;
    unsigned char type; // 0 - scen, 1 - out, 2 - town
    location where;
    }pending_special_type;

//text buffer struct (i.e bottom right window's content)
typedef struct {
	char line[50];
	} buf_line;

#endif
