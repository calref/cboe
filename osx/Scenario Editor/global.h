#define	SLEEP_TICKS		2L
#define	MOUSE_REGION	0L
#define DRAG_EDGE		15
#define IN_FRONT	(WindowPtr)-1L
#define	NIL				0L

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
#define	TER_RECT_UL_Y	25

#define	CDGT	cd_retrieve_text_edit_str
#define	CDGN	cd_retrieve_text_edit_num
#define	CDST	cd_set_text_edit_str
#define	CDSN	cd_set_text_edit_num

typedef struct {
	char x,y;
	} location;

typedef struct {
	short type,sd1,sd2,pic,m1,m2,ex1a,ex1b,ex2a,ex2b,jumpto;
	} special_node_type;

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
	
typedef	struct {
	unsigned char monst[4];
	} wandering_type;

typedef struct {
	unsigned char monst[7];
	unsigned char friendly[3];
	short spec_on_meet,spec_on_win,spec_on_flee,cant_flee;
	short end_spec1,end_spec2;
} out_wandering_type;

typedef struct {
	unsigned char	terrain[48][48];
	location	special_locs[18];
	unsigned char		special_id[18];
	location	exit_locs[8];
	char		exit_dests[8];
	location	sign_locs[8];
	out_wandering_type	wandering[4],special_enc[4];
	location	wandering_locs[4];
	Rect info_rect[8];
	unsigned char strlens[180];
	special_node_type specials[60];
	} outdoor_record_type;

typedef struct {
	unsigned char number;
	unsigned char start_attitude;
	location start_loc;
	unsigned char  mobile;
	unsigned char  time_flag;
	unsigned char  extra1,extra2;
	short spec1, spec2;
	char spec_enc_code,time_code;
	short monster_time,personality;
	short special_on_kill,facial_pic;
	
	} creature_start_type;


typedef struct {
	short variety, item_level;
	char awkward, bonus, protection, charges, type;
	unsigned char graphic_num,ability, type_flag, is_special;
	short value;
	Boolean identified, magic;
	unsigned char weight, description_flag;
	char full_name[25], name[15];
	unsigned char reserved1,reserved2;
	unsigned char magic_use_type, ability_strength, treas_class, real_abil;
} short_item_record_type;


typedef struct {
	short variety, item_level;
	char awkward, bonus, protection, charges, type, magic_use_type;
	unsigned char graphic_num,ability, ability_strength,type_flag, is_special;
	short value;
	unsigned char weight, special_class;
	location item_loc;
	char full_name[25], name[15];
	unsigned char treas_class,item_properties,reserved1,reserved2;
} item_record_type;

typedef struct {
	location item_loc;
	short item_code,ability;
	unsigned char charges,always_there,property,contained;
	} preset_item_type;
	
typedef struct {
	location field_loc;
	short field_type;
	} preset_field_type;
	
typedef struct {
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
	Rect in_town_rect;
	preset_item_type preset_items[64];
	short max_num_monst;
	preset_field_type preset_fields[50];
	short spec_on_entry,spec_on_entry_if_dead;
	short timer_spec_times[8];
	short timer_specs[8];
	unsigned char strlens[180];
	special_node_type specials[100];
	unsigned char specials1,specials2,res1,res2;
	short difficulty;
	} town_record_type;

typedef struct {
	unsigned char terrain[64][64];
	Rect room_rect[16];
	creature_start_type creatures[60];
	unsigned char lighting[8][64];
	} big_tr_type;

typedef struct {
	unsigned char terrain[48][48];
	Rect room_rect[16];
	creature_start_type creatures[40];
	unsigned char lighting[6][48];
	} ave_tr_type;
	
typedef struct {
	unsigned char terrain[32][32];
	Rect room_rect[16];
	creature_start_type creatures[30];
	unsigned char lighting[4][32];
	} tiny_tr_type;	

typedef struct {
	short block_type;
	short block_destroy_time;
	char block_alignment;
	char block_key_time;
	location block_loc;
	} city_block_type;
	
typedef struct {
	Rect what_rect;
	unsigned char ter_type;
	unsigned char hollow;
	} city_ter_rect_type;
	
typedef struct {
	creature_start_type creatures[30];
	city_block_type city_block[15];
	city_ter_rect_type city_ter_rect[10];
	} template_town_type;

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
	unsigned char town_size[200];
	unsigned char town_hidden[200];
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
	Rect store_item_rects[3];
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

// for game
typedef struct {
	short personality;
	short str1,str2;
	} talk_save_type;



typedef struct {
	short active,attitude;
	unsigned char number;
	location m_loc;
	monster_record_type m_d;
	short mobile;
	short summoned;
	creature_start_type monst_start;
	} creature_data_type;

typedef struct {
	creature_data_type dudes[60];
	short which_town;
	short friendly;
	} creature_list_type;

typedef struct {
	Boolean exists;
	short direction;
	out_wandering_type what_monst;
	location which_sector,m_loc;	
	} outdoor_creature_type;
	
typedef struct {
	long age;
	short gold,food;
	unsigned char stuff_done[310][10],item_taken[200][8];
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
	char spec_items[50];
	} party_record_type;

typedef struct {
	char strings_ls[NLS][40];
	char strings_rs[NRS][40];
	char town_strs[180][256];
	char out_strs[120][256];
	char scen_strs[270][256];
	char talk_strs[170][256];
	scen_item_data_type scen_item_list;
	} piles_of_stuff_dumping_type;
	
short get_ran (short times,short  min,short  max);
short s_pow(short x,short y);
short dist(location p1,location p2);

short max(short a,short b);
short min(short a,short b);
short minmax(short min,short max,short k);
//short abs(short x);
