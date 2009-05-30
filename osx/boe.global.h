
#include <vector>
#include <string>
#include <sstream>
#include "boe.consts.h"

//#define EXILE_BIG_GUNS 1

#define	SLEEP_TICKS		2L
#define	MOUSE_REGION	0L
#define DRAG_EDGE		15
#define IN_FRONT	(WindowPtr)-1L

#define NUM_TOWN_ITEMS	115


#define	DISPLAY_LEFT	23
#define	DISPLAY_TOP		23
#define	BITMAP_WIDTH	28
#define	BITMAP_HEIGHT	36

#define	STORED_GRAPHICS	240

#define	PC_WIN_UL_X	291
#define	PC_WIN_UL_Y	5
#define	ITEM_WIN_UL_X	291
#define	ITEM_WIN_UL_Y	130
#define	TEXT_WIN_UL_X	291
#define	TEXT_WIN_UL_Y	283
#define	NUM_BUTTONS		15
#define ASB	add_string_to_buf
#define	PSD	univ.party.stuff_done
#define ADVEN univ.party.adven
#define	DES	display_enc_string
#define	D2ES display_2_enc_string

#define	NUM_MONST_G	173
#define	NUM_TER_G	251
#define	NUM_ITEM_G	120
#define	NUM_FACE_G	80
#define	NUM_DLOG_G	28

#define	CDGT	cd_retrieve_text_edit_str
#define	CDGN	cd_retrieve_text_edit_num
#define	CDST	cd_set_text_edit_str
#define	CDSN	cd_set_text_edit_num

//typedef struct {
//	short personality,type;
//	char link1[4],link2[4];
//	short extras[4];
//	} talking_node_type;
//
//typedef struct {
//	unsigned char strlens[200];
//	talking_node_type talk_nodes[60];
//	} talking_record_type;
//	
//typedef	struct {
//	unsigned char monst[4];
//	} wandering_type;
//
//typedef struct {
//	unsigned char monst[7];
//	unsigned char friendly[3];
//	short spec_on_meet,spec_on_win,spec_on_flee,cant_flee;
//	short end_spec1,end_spec2;
//} out_wandering_type;
//
//typedef struct {
//	unsigned char	terrain[48][48];
//	location	special_locs[18];
//	unsigned char		special_id[18];
//	location	exit_locs[8];
//	char		exit_dests[8];
//	location	sign_locs[8];
//	out_wandering_type	wandering[4],special_enc[4];
//	location	wandering_locs[4];
//	Rect info_rect[8];
//	unsigned char strlens[180];
//	cSpecial specials[60];
//	} outdoor_record_type;
//
//typedef struct {
//	unsigned char number;
//	unsigned char start_attitude;
//	location start_loc;
//	unsigned char  mobile;
//	unsigned char  time_flag;
//	unsigned char  extra1,extra2;
//	short spec1, spec2;
//	char spec_enc_code,time_code;
//	short monster_time,personality;
//	short special_on_kill,facial_pic;
//	
//	} creature_start_type;
//
//typedef struct {
//	location item_loc;
//	short item_code,ability;
//	unsigned char charges,always_there,property,contained;
//	} preset_item_type;
//	
//typedef struct {
//	location field_loc;
//	short field_type;
//	} preset_field_type;
//	
//typedef struct {
//	short town_chop_time,town_chop_key;
//	wandering_type	wandering[4];
//	location	wandering_locs[4];
//	location	special_locs[50];
//	unsigned char		spec_id[50];
//	location	sign_locs[15];
//	short		lighting;
//	location start_locs[4];
//	location exit_locs[4];
//	short exit_specs[4];
//	Rect in_town_rect;
//	preset_item_type preset_items[64];
//	short max_num_monst;
//	preset_field_type preset_fields[50];
//	short spec_on_entry,spec_on_entry_if_dead;
//	short timer_spec_times[8];
//	short timer_specs[8];
//	unsigned char strlens[180];
//	cSpecial specials[100];
//	unsigned char specials1,specials2,res1,res2;
//	short difficulty;
//	} town_record_type;
//
//
//
//typedef struct {
//	unsigned char terrain[64][64];
//	Rect room_rect[16];
//	creature_start_type creatures[60];
//	unsigned char lighting[8][64];
//	} big_tr_type;
//
//typedef struct {
//	unsigned char terrain[48][48];
//	Rect room_rect[16];
//	creature_start_type creatures[40];
//	unsigned char lighting[6][48];
//	} ave_tr_type;
//	
//typedef struct {
//	unsigned char terrain[32][32];
//	Rect room_rect[16];
//	creature_start_type creatures[30];
//	unsigned char lighting[4][32];
//	} tiny_tr_type;	
//
//typedef struct {
//	short block_type;
//	short block_destroy_time;
//	char block_alignment;
//	char block_key_time;
//	location block_loc;
//	} city_block_type;
//	
//typedef struct {
//	Rect what_rect;
//	unsigned char ter_type;
//	unsigned char hollow;
//	} city_ter_rect_type;
//	
//typedef struct {
//	creature_start_type creatures[30];
//	city_block_type city_block[15];
//	city_ter_rect_type city_ter_rect[10];
//	} template_town_type;
//
//typedef struct {
//	cItemRec scen_items[400];
//	char monst_names[256][20];
//	char ter_names[256][30];
//	} scen_item_data_type;
	
//struct creature_data_type {
//	short active,attitude;
//	unsigned char number;
//	location m_loc;
//	cMonster m_d;
//	bool mobile;
//	short summoned;
//	cTown::cCreature monst_start;
//};

struct scen_header_type {
	unsigned char flag1, flag2, flag3, flag4;
	unsigned char ver[3],min_run_ver,prog_make_ver[3],num_towns;
	unsigned char out_width,out_height,difficulty,intro_pic,default_ground;
};

struct scen_header_str_type{
	std::string name, who1, who2, file;
	//scen_header_str_type(){}
	scen_header_str_type& operator = (const scen_header_str_type& other) {
		name = other.name;
		who1 = other.who1;
		who2 = other.who2;
		file = other.file;
		return *this;
	}
};

// for game
//struct talk_save_type {
//	short personality;
//	short town_num;
//	short str1,str2;
//};
//
//struct creature_list_type {
//	creature_data_type dudes[60];
//	short which_town;
//	short friendly;
//};
//
//struct current_town_type {
//	short town_num, difficulty;
//	cTown* town;
//	char explored[64][64];
//	bool	hostile;
//	creature_list_type	monst;
//	bool	in_boat;
//	location p_loc;
//	cSpeech* cur_talk;
//	short cur_talk_loaded;
//};
//
//struct outdoor_creature_type {
//	bool exists;
//	short direction;
//	cOutdoors::cWandering what_monst;
//	location which_sector,m_loc;	
//};
//	
//struct party_record_type {
//	long age;
//	short gold,food;
//	unsigned char stuff_done[310][10],item_taken[200][8];
//	short light_level;
//	location outdoor_corner,i_w_c,p_loc,loc_in_sec;
//	cVehicle boats[30];
//	cVehicle horses[30];
//	creature_list_type creature_save[4];
//	short in_boat,in_horse;
//	outdoor_creature_type out_c[10];
//	cItemRec magic_store_items[5][10];
//	short imprisoned_monst[4];
//	char m_seen[256];
//	char journal_str[50];
//	short journal_day[50];
//	short special_notes_str[140][2];
//	talk_save_type talk_save[120];
//	short direction,at_which_save_slot;
//	char alchemy[20];
//	bool can_find_town[200];
//	short key_times[100];
//	short party_event_timers[30];
//	short global_or_town[30];
//	short node_to_call[30];
//	char spec_items[50],help_received[120];
//	short m_killed[200];
//	long total_m_killed,total_dam_done,total_xp_gained,total_dam_taken;
//	char scen_name[256];
//};
//
//struct stored_town_maps_type {
//	char town_maps[200][8][64];
//}; 

//typedef struct {
////	char town_strs[180][256];
//	//char out_strs[120][256];
////	char scen_strs[270][256];
////	char talk_strs[170][256];
//	std::vector<std::string*> scen_header_strs;
//	std::vector<std::string> scen_names;
////	scen_item_data_type scen_item_list;
//	stored_town_maps_type town_maps;
//	} piles_of_stuff_dumping_type;

//typedef struct {
//	char out_strs[9][256];
//	} outdoor_strs_type;
//struct pc_record_type {
//	short main_status;
//	char name[20];
//	short skills[30];
//	short max_health,cur_health,max_sp,cur_sp,experience,skill_pts,level;
//	short status[15];
//	cItemRec items[24];
//	bool equip[24];
//	bool priest_spells[62],mage_spells[62];
//	short which_graphic,weap_poisoned;
//	bool advan[15],traits[15];
//	short race,exp_adj,direction;
//};
//
//struct setup_save_type {
//	unsigned char setup[4][64][64];
//};
//	
//
//struct town_item_list {
//	cItemRec items[NUM_TOWN_ITEMS];
//};
//
//struct flag_type {
//	short i;
//};

struct effect_pat_type {
	unsigned char pattern[9][9];
};

//typedef struct stored_outdoor_maps_type {
//	char outdoor_maps[100][6][48];
//}; 

//typedef struct {
//long l[10];
//} PrefRecord,*PrefPtr,**PrefHandle;
//
