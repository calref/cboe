//#include <vector>
//#include <string>
//#include "scen.consts.h"

#define	SLEEP_TICKS		2L
#define	MOUSE_REGION	0L
#define DRAG_EDGE		15
#define IN_FRONT	(WindowPtr)-1L

#define	DISPLAY_LEFT	23
#define	DISPLAY_TOP		23
#define	BITMAP_WIDTH	28
#define	BITMAP_HEIGHT	36
#define	NUM_BUTTONS		15

#define	NUM_DLOG_B		53

#define NLS	25
// number of left slots for buttons
#define	NRS	400
// number of right slots for scrolling list
#define NRSONPAGE	33
// number of right slots for scrolling list on page at 1 time

#define	RIGHT_AREA_UL_X	287
#define	RIGHT_AREA_UL_Y	5
#define	RIGHT_AREA_WIDTH	290
#define	RIGHT_AREA_HEIGHT	400

#define	TER_RECT_UL_X	6
#define	TER_RECT_UL_Y	19 // Formerly 25

#define	CDGT	cd_retrieve_text_edit_str
#define	CDGN	cd_retrieve_text_edit_num
#define	CDST	cd_set_text_edit_str
#define	CDSN	cd_set_text_edit_num

enum eScenMode {
	MODE_DRAWING = 0,
	MODE_SET_WANDER_POINTS = 2,
	MODE_ROOM_RECT = 3,
	MODE_PLACE_ITEM = 4,
	MODE_EDIT_ITEM = 5,
	MODE_TOGGLE_SPECIAL_DOT = 6, // I suspect that's what this was for, anyway – it currently does nothing
	MODE_SET_TOWN_START = 7, // unused; for something I'd like to add
	MODE_PLACE_SAME_CREATURE = 8,
	MODE_SET_TOWN_RECT = 9,
	MODE_PLACE_NORTH_ENTRANCE = 10,
	MODE_PLACE_EAST_ENTRANCE = 11,
	MODE_PLACE_SOUTH_ENTRANCE = 12,
	MODE_PLACE_WEST_ENTRANCE = 13,
	MODE_PLACE_WEB = 20,
	MODE_PLACE_CRATE = 21,
	MODE_PLACE_BARREL = 22,
	MODE_PLACE_FIRE_BARRIER = 23,
	MODE_PLACE_FORCE_BARRIER = 24,
	MODE_PLACE_QUICKFIRE = 25,
	MODE_CLEAR_FIELDS = 26,
	MODE_PLACE_STONE_BLOCK = 27, // unused; for something I'd like to add
	MODE_PLACE_CREATURE = 28,
	MODE_LARGE_PAINTBRUSH = 29, // uncertain
	MODE_SMALL_PAINTBRUSH = 30, // uncertain
	MODE_LARGE_SPRAYCAN = 31, // uncertain
	MODE_SMALL_SPRAYCAN = 32, // uncertain
	MODE_PLACE_SFX = 33, // eg bloodstain or ashes
	MODE_EYEDROPPER = 34,
	MODE_HOLLOW_RECT = 35,
	MODE_FILLED_RECT = 36,
	MODE_ERASER = 37,
	MODE_EDIT_SIGN = 38,
	MODE_EDIT_CREATURE = 39,
	MODE_EDIT_SPECIAL = 40, // or create a special if there's none there
	MODE_COPY_SPECIAL = 41,
	MODE_PASTE_SPECIAL = 42,
	MODE_ERASE_SPECIAL = 43,
	MODE_PLACE_SPECIAL = 44, // or set the special to be called if there's already one there
	MODE_EDIT_TOWN_ENTRANCE = 45, // for outdoors
	MODE_PLACE_SAME_ITEM = 46,
	MODE_SET_OUT_START = 47,
	MODE_ERASE_CREATURE = 48,
	MODE_ERASE_ITEM = 49,
	MODE_MAIN_SCREEN = 60,
	MODE_INTRO_SCREEN = 61,
	MODE_EDIT_TYPES = 62, // currently only used for editing terrain, but I'd like to use it for editing monsters and items too.
};

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

//typedef struct {
//	cItemRec scen_items[400];
//	char monst_names[256][20];
//	char ter_names[256][30];
//	} scen_item_data_type;

// for game
//typedef struct {
//	short personality;
//	short str1,str2;
//	} talk_save_type;



//typedef struct {
//	short active,attitude;
//	unsigned char number;
//	location m_loc;
//	cMonster m_d;
//	short mobile;
//	short summoned;
//	cTown::cCreature monst_start;
//	} creature_data_type;

//typedef struct {
//	creature_data_type dudes[60];
//	short which_town;
//	short friendly;
//	} creature_list_type;

//typedef struct {
//	Boolean exists;
//	short direction;
//	cOutdoors::cWandering what_monst;
//	location which_sector,m_loc;	
//	} outdoor_creature_type;
	
//typedef struct {
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
//	char spec_items[50];
//	} party_record_type;

//typedef struct {
//	char strings_ls[NLS][40];
//	char strings_rs[NRS][40];
//	char town_strs[180][256];
//	char out_strs[120][256];
//	char scen_strs[270][256];
//	char talk_strs[170][256];
//	scen_item_data_type scen_item_list;
//} piles_of_stuff_dumping_type;
