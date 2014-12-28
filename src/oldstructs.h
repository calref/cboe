/*
 *  oldstructs.h
 *  BoE
 *
 *  Created by Celtic Minstrel on 21/04/09.
 *
 */

#ifndef OLDSTRUCTS_H
#define OLDSTRUCTS_H
#include <cstdint>

namespace legacy {
	// These definitions are basically copied from MacTypes.h
	extern "C" {
		struct Rect {
			int16_t               top;
			int16_t               left;
			int16_t               bottom;
			int16_t               right;
		};
		typedef struct Rect                     Rect;
		typedef uint8_t                   Boolean;
	}
	using pad_t = uint8_t; // To make the padding bytes super-explicit
//#define NLS	25
//	// number of left slots for buttons
//#define	NRS	400
//	// number of right slots for scrolling list
//#define NRSONPAGE	33
//	// number of right slots for scrolling list on page at 1 time
	
#pragma pack(push,1)
	struct location {
		int8_t x,y;
	};
	
	struct special_node_type {
		int16_t type,sd1,sd2,pic,m1,m2,ex1a,ex1b,ex2a,ex2b,jumpto;
	};
	
	struct talking_node_type {
		int16_t personality,type;
		int8_t link1[4],link2[4];
		int16_t extras[4];
	};
	
	struct talking_record_type {
		uint8_t strlens[200];
		talking_node_type talk_nodes[60];
	};
	
	struct terrain_type_type {
		int16_t picture;
		uint8_t blockage,flag1,flag2,special,trans_to_what,fly_over,boat_over;
		uint8_t block_horse,light_radius,step_sound,shortcut_key,res1,res2,res3;
	};
	
	struct wandering_type {
		uint8_t monst[4];
	};
	
	struct out_wandering_type {
		uint8_t monst[7];
		uint8_t friendly[3];
		int16_t spec_on_meet,spec_on_win,spec_on_flee,cant_flee;
		int16_t end_spec1,end_spec2;
	};
	
	struct outdoor_record_type {
		uint8_t	terrain[48][48];
		location	special_locs[18];
		uint8_t		special_id[18];
		location	exit_locs[8];
		int8_t		exit_dests[8];
		location	sign_locs[8];
		out_wandering_type	wandering[4],special_enc[4];
		location	wandering_locs[4];
		Rect info_rect[8];
		uint8_t strlens[180];
		special_node_type specials[60];
	};
	
	struct creature_start_type {
		uint8_t number;
		uint8_t start_attitude;
		location start_loc;
		uint8_t  mobile;
		uint8_t  time_flag;
		uint8_t  extra1,extra2;
		int16_t spec1, spec2;
		int8_t spec_enc_code,time_code;
		int16_t monster_time,personality;
		int16_t special_on_kill,facial_pic;
	};
	
	struct short_item_record_type {
		int16_t variety, item_level;
		int8_t awkward, bonus, protection, charges, type;
		uint8_t graphic_num,ability, type_flag, is_special;
		int16_t value;
		Boolean identified, magic;
		uint8_t weight, description_flag;
		char full_name[25], name[15];
		uint8_t reserved1,reserved2;
		uint8_t magic_use_type, ability_strength, treas_class, real_abil;
	};
	
	struct item_record_type {
		int16_t variety, item_level;
		int8_t awkward, bonus, protection, charges, type, magic_use_type;
		uint8_t graphic_num,ability, ability_strength,type_flag, is_special;
		pad_t xxx;
		int16_t value;
		uint8_t weight, special_class;
		location item_loc;
		char full_name[25], name[15];
		uint8_t treas_class,item_properties,reserved1,reserved2;
	};
	
	struct preset_item_type {
		location item_loc;
		int16_t item_code,ability;
		uint8_t charges,always_there,property,contained;
	};
	
	struct preset_field_type {
		location field_loc;
		int16_t field_type;
	};
	
	struct town_record_type {
		int16_t town_chop_time,town_chop_key;
		wandering_type	wandering[4];
		location	wandering_locs[4];
		location	special_locs[50];
		uint8_t		spec_id[50];
		location	sign_locs[15];
		int16_t		lighting;
		location start_locs[4];
		location exit_locs[4];
		int16_t exit_specs[4];
		Rect in_town_rect;
		preset_item_type preset_items[64];
		int16_t max_num_monst;
		preset_field_type preset_fields[50];
		int16_t spec_on_entry,spec_on_entry_if_dead;
		int16_t timer_spec_times[8];
		int16_t timer_specs[8];
		uint8_t strlens[180];
		special_node_type specials[100];
		uint8_t specials1,specials2,res1,res2;
		int16_t difficulty;
	};
	
	struct big_tr_type {
		uint8_t terrain[64][64];
		Rect room_rect[16];
		creature_start_type creatures[60];
		uint8_t lighting[8][64];
	};
	
	struct ave_tr_type {
		uint8_t terrain[48][48];
		Rect room_rect[16];
		creature_start_type creatures[40];
		uint8_t lighting[6][48];
	};
	
	struct tiny_tr_type {
		uint8_t terrain[32][32];
		Rect room_rect[16];
		creature_start_type creatures[30];
		uint8_t lighting[4][32];
	};
	
	struct city_block_type {
		int16_t block_type;
		int16_t block_destroy_time;
		int8_t block_alignment;
		int8_t block_key_time;
		location block_loc;
	};
	
	struct city_ter_rect_type {
		Rect what_rect;
		uint8_t ter_type;
		uint8_t hollow;
	};
	
	struct template_town_type {
		creature_start_type creatures[30];
		city_block_type city_block[15];
		city_ter_rect_type city_ter_rect[10];
	};
	
	struct scen_item_data_type {
		item_record_type scen_items[400];
		char monst_names[256][20];
		char ter_names[256][30];
	};
	
	struct item_storage_shortcut_type {
		int16_t ter_type,item_num[10],item_odds[10],property;
	};
	
	struct monster_record_type {
		uint8_t m_num,level,m_name[26];
		int16_t health,m_health,mp,max_mp;
		uint8_t armor,skill;
		int16_t a[3];
		uint8_t a1_type,a23_type,m_type,speed,ap,mu,cl,breath,breath_type,treasure,spec_skill,poison;
		int16_t morale,m_morale;
		int16_t corpse_item,corpse_item_chance;
		int16_t status[15];
		uint8_t direction,immunities,x_width,y_width,radiate_1,radiate_2;
		uint8_t default_attitude,summon_type,default_facial_pic,res1,res2,res3;
		int16_t picture_num;
		
	};
	
	struct horse_record_type {
		location horse_loc,horse_loc_in_sec,horse_sector;
		int16_t which_town;
		Boolean exists,property;
	};
	
	struct boat_record_type {
		location boat_loc,boat_loc_in_sec,boat_sector;
		int16_t which_town;
		Boolean exists,property;
	};
	
	struct talk_save_type {
		int16_t personality;
		int16_t town_num;
		int16_t str1,str2;
	};
	
	struct creature_data_type {
		int16_t active,attitude;
		uint8_t number;
		pad_t xxx;
		location m_loc;
		monster_record_type m_d;
		int16_t mobile;
		int16_t summoned;
		creature_start_type monst_start;
	};
	
	struct creature_list_type {
		creature_data_type dudes[60];
		int16_t which_town;
		int16_t friendly;
	};
	
	struct outdoor_creature_type {
		Boolean exists;
		pad_t xxx;
		int16_t direction;
		out_wandering_type what_monst;
		location which_sector,m_loc;	
	};
	
	struct party_record_type {
		int32_t age;
		int16_t gold,food;
		uint8_t stuff_done[310][10],item_taken[200][8];
		int16_t light_level;
		location outdoor_corner,i_w_c,p_loc,loc_in_sec;
		boat_record_type boats[30];
		horse_record_type horses[30];
		//int8_t pad1[8];
		creature_list_type creature_save[4];
		int16_t in_boat,in_horse;
		outdoor_creature_type out_c[10];
		item_record_type magic_store_items[5][10];
		int16_t imprisoned_monst[4];
		int8_t m_seen[256];
		int8_t journal_str[50];
		int16_t journal_day[50];
		int16_t special_notes_str[140][2];
		talk_save_type talk_save[120];
		int16_t direction,at_which_save_slot;
		int8_t alchemy[20];
		Boolean can_find_town[200];
		int16_t key_times[100];
		int16_t party_event_timers[30];
		int16_t global_or_town[30];
		int16_t node_to_call[30];
		int8_t spec_items[50],help_received[120];
		int16_t m_killed[200];
		int32_t total_m_killed,total_dam_done,total_xp_gained,total_dam_taken;
		char scen_name[256];
	};
	
	struct scenario_data_type {
		uint8_t out_width,out_height,difficulty,intro_pic,default_ground;
		uint8_t town_size[200];
		uint8_t town_hidden[200];
		int16_t flag_a;
		int16_t intro_mess_pic,intro_mess_len;
		pad_t xxx;
		location where_start;
		location out_sec_start,out_start;
		int16_t which_town_start;
		int16_t flag_b;
		int16_t town_data_size[200][5];
		int16_t town_to_add_to[10];
		int16_t flag_to_add_to_town[10][2];
		int16_t flag_c;
		int16_t out_data_size[100][2];
		Rect store_item_rects[3];
		int16_t store_item_towns[3];
		int16_t flag_e;
		int16_t special_items[50];
		int16_t special_item_special[50];
		int16_t rating,uses_custom_graphics;
		int16_t flag_f;
		monster_record_type scen_monsters[256];
		boat_record_type scen_boats[30];
		horse_record_type scen_horses[30];
		int16_t flag_g;
		terrain_type_type ter_types[256];
		int16_t scenario_timer_times[20];
		int16_t scenario_timer_specs[20];
		int16_t flag_h;
		special_node_type scen_specials[256];
		item_storage_shortcut_type storage_shortcuts[10];
		int16_t flag_d;
		uint8_t scen_str_len[300];
		int16_t flag_i;
		location last_out_edited;
		int16_t last_town_edited;
	};
	
	struct setup_save_type {
		uint8_t setup[4][64][64];
	};
	
	struct pc_record_type {
		int16_t main_status;
		char name[20];
		int16_t skills[30];
		int16_t max_health,cur_health,max_sp,cur_sp,experience,skill_pts,level;
		int16_t status[15];
		item_record_type items[24];
		Boolean equip[24];
		Boolean priest_spells[62],mage_spells[62];
		int16_t which_graphic,weap_poisoned;
		Boolean advan[15],traits[15];
		int16_t race,exp_adj,direction;
	};
	
	struct town_item_list {
		item_record_type items[115];
	};
	
	struct stored_town_maps_type {
		int8_t town_maps[200][8][64];
	};
	
	struct stored_outdoor_maps_type {
		int8_t outdoor_maps[100][6][48];
	};
	
	struct stored_items_list_type {
		item_record_type items[115];
	};
	
	struct current_town_type {
		int16_t town_num, difficulty;
		town_record_type	town;
		int8_t explored[64][64];
		Boolean hostile;
		creature_list_type	monst;
		Boolean in_boat;
		location p_loc;
	};
	
	struct out_info_type {
		int8_t expl[96][96];
	};
	
//	struct piles_of_stuff_dumping_type {
//		int8_t strings_ls[NLS][40];
//		int8_t strings_rs[NRS][40];
//		int8_t town_strs[180][256];
//		int8_t out_strs[120][256];
//		int8_t scen_strs[270][256];
//		int8_t talk_strs[170][256];
//		scen_item_data_type scen_item_list;
//	};
#pragma pack(pop)
};

#endif
