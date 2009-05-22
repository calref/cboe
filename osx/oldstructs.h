/*
 *  oldstructs.h
 *  BoE
 *
 *  Created by Celtic Minstrel on 21/04/09.
 *
 */

#ifndef OLDSTRUCTS_H
#define OLDSTRUCTS_H

namespace legacy {
//#define NLS	25
//	// number of left slots for buttons
//#define	NRS	400
//	// number of right slots for scrolling list
//#define NRSONPAGE	33
//	// number of right slots for scrolling list on page at 1 time
	
	struct location {
		char x,y		__attribute__((aligned(2)));
	};
	
	struct special_node_type {
		short type,sd1,sd2,pic,m1,m2,ex1a,ex1b,ex2a,ex2b,jumpto		__attribute__((aligned(2)));
	};
	
	struct talking_node_type {
		short personality,type		__attribute__((aligned(2)));
		char link1[4],link2[4]		__attribute__((aligned(2)));
		short extras[4]		__attribute__((aligned(2)));
	};
	
	struct talking_record_type {
		unsigned char strlens[200]		__attribute__((aligned(2)));
		talking_node_type talk_nodes[60]		__attribute__((aligned(2)));
	};
	
	struct terrain_type_type {
		short picture		__attribute__((aligned(2)));
		unsigned char blockage,flag1,flag2,special,trans_to_what,fly_over,boat_over		__attribute__((aligned(2)));
		unsigned char block_horse,light_radius,step_sound,shortcut_key,res1,res2,res3		__attribute__((aligned(2)));
	};
	
	struct wandering_type {
		unsigned char monst[4]		__attribute__((aligned(2)));
	};
	
	struct out_wandering_type {
		unsigned char monst[7]		__attribute__((aligned(2)));
		unsigned char friendly[3]		__attribute__((aligned(2)));
		short spec_on_meet,spec_on_win,spec_on_flee,cant_flee		__attribute__((aligned(2)));
		short end_spec1,end_spec2		__attribute__((aligned(2)));
	};
	
	struct outdoor_record_type {
		unsigned char	terrain[48][48]		__attribute__((aligned(2)));
		location	special_locs[18]		__attribute__((aligned(2)));
		unsigned char		special_id[18]		__attribute__((aligned(2)));
		location	exit_locs[8]		__attribute__((aligned(2)));
		char		exit_dests[8]		__attribute__((aligned(2)));
		location	sign_locs[8]		__attribute__((aligned(2)));
		out_wandering_type	wandering[4],special_enc[4]		__attribute__((aligned(2)));
		location	wandering_locs[4]		__attribute__((aligned(2)));
		Rect info_rect[8]		__attribute__((aligned(2)));
		unsigned char strlens[180]		__attribute__((aligned(2)));
		special_node_type specials[60]		__attribute__((aligned(2)));
	};
	
	struct creature_start_type {
		unsigned char number		__attribute__((aligned(2)));
		unsigned char start_attitude		__attribute__((aligned(2)));
		location start_loc		__attribute__((aligned(2)));
		unsigned char  mobile		__attribute__((aligned(2)));
		unsigned char  time_flag		__attribute__((aligned(2)));
		unsigned char  extra1,extra2		__attribute__((aligned(2)));
		short spec1, spec2		__attribute__((aligned(2)));
		char spec_enc_code,time_code		__attribute__((aligned(2)));
		short monster_time,personality		__attribute__((aligned(2)));
		short special_on_kill,facial_pic		__attribute__((aligned(2)));
	};
	
	struct short_item_record_type {
		short variety, item_level		__attribute__((aligned(2)));
		char awkward, bonus, protection, charges, type		__attribute__((aligned(2)));
		unsigned char graphic_num,ability, type_flag, is_special		__attribute__((aligned(2)));
		short value		__attribute__((aligned(2)));
		bool identified, magic		__attribute__((aligned(2)));
		unsigned char weight, description_flag		__attribute__((aligned(2)));
		char full_name[25], name[15]		__attribute__((aligned(2)));
		unsigned char reserved1,reserved2		__attribute__((aligned(2)));
		unsigned char magic_use_type, ability_strength, treas_class, real_abil		__attribute__((aligned(2)));
	};
	
	struct item_record_type {
		short variety, item_level		__attribute__((aligned(2)));
		char awkward, bonus, protection, charges, type, magic_use_type		__attribute__((aligned(2)));
		unsigned char graphic_num,ability, ability_strength,type_flag, is_special		__attribute__((aligned(2)));
		short value		__attribute__((aligned(2)));
		unsigned char weight, special_class		__attribute__((aligned(2)));
		location item_loc		__attribute__((aligned(2)));
		char full_name[25], name[15]		__attribute__((aligned(2)));
		unsigned char treas_class,item_properties,reserved1,reserved2		__attribute__((aligned(2)));
	};
	
	struct preset_item_type {
		location item_loc		__attribute__((aligned(2)));
		short item_code,ability		__attribute__((aligned(2)));
		unsigned char charges,always_there,property,contained		__attribute__((aligned(2)));
	};
	
	struct preset_field_type {
		location field_loc		__attribute__((aligned(2)));
		short field_type		__attribute__((aligned(2)));
	};
	
	struct town_record_type {
		short town_chop_time,town_chop_key		__attribute__((aligned(2)));
		wandering_type	wandering[4]		__attribute__((aligned(2)));
		location	wandering_locs[4]		__attribute__((aligned(2)));
		location	special_locs[50]		__attribute__((aligned(2)));
		unsigned char		spec_id[50]		__attribute__((aligned(2)));
		location	sign_locs[15]		__attribute__((aligned(2)));
		short		lighting		__attribute__((aligned(2)));
		location start_locs[4]		__attribute__((aligned(2)));
		location exit_locs[4]		__attribute__((aligned(2)));
		short exit_specs[4]		__attribute__((aligned(2)));
		Rect in_town_rect		__attribute__((aligned(2)));
		preset_item_type preset_items[64]		__attribute__((aligned(2)));
		short max_num_monst		__attribute__((aligned(2)));
		preset_field_type preset_fields[50]		__attribute__((aligned(2)));
		short spec_on_entry,spec_on_entry_if_dead		__attribute__((aligned(2)));
		short timer_spec_times[8]		__attribute__((aligned(2)));
		short timer_specs[8]		__attribute__((aligned(2)));
		unsigned char strlens[180]		__attribute__((aligned(2)));
		special_node_type specials[100]		__attribute__((aligned(2)));
		unsigned char specials1,specials2,res1,res2		__attribute__((aligned(2)));
		short difficulty		__attribute__((aligned(2)));
	};
	
	struct big_tr_type {
		unsigned char terrain[64][64]		__attribute__((aligned(2)));
		Rect room_rect[16]		__attribute__((aligned(2)));
		creature_start_type creatures[60]		__attribute__((aligned(2)));
		unsigned char lighting[8][64]		__attribute__((aligned(2)));
	};
	
	struct ave_tr_type {
		unsigned char terrain[48][48]		__attribute__((aligned(2)));
		Rect room_rect[16]		__attribute__((aligned(2)));
		creature_start_type creatures[40]		__attribute__((aligned(2)));
		unsigned char lighting[6][48]		__attribute__((aligned(2)));
	};
	
	struct tiny_tr_type {
		unsigned char terrain[32][32]		__attribute__((aligned(2)));
		Rect room_rect[16]		__attribute__((aligned(2)));
		creature_start_type creatures[30]		__attribute__((aligned(2)));
		unsigned char lighting[4][32]		__attribute__((aligned(2)));
	};	
	
	struct city_block_type {
		short block_type		__attribute__((aligned(2)));
		short block_destroy_time		__attribute__((aligned(2)));
		char block_alignment		__attribute__((aligned(2)));
		char block_key_time		__attribute__((aligned(2)));
		location block_loc		__attribute__((aligned(2)));
	};
	
	struct city_ter_rect_type {
		Rect what_rect		__attribute__((aligned(2)));
		unsigned char ter_type		__attribute__((aligned(2)));
		unsigned char hollow		__attribute__((aligned(2)));
	};
	
	struct template_town_type {
		creature_start_type creatures[30]		__attribute__((aligned(2)));
		city_block_type city_block[15]		__attribute__((aligned(2)));
		city_ter_rect_type city_ter_rect[10]		__attribute__((aligned(2)));
	};
	
	struct scen_item_data_type {
		item_record_type scen_items[400]		__attribute__((aligned(2)));
		char monst_names[256][20]		__attribute__((aligned(2)));
		char ter_names[256][30]		__attribute__((aligned(2)));
	};
	
	struct item_storage_shortcut_type {
		short ter_type,item_num[10],item_odds[10],property		__attribute__((aligned(2)));
	};
	
	struct monster_record_type {
		unsigned char m_num,level,m_name[26]		__attribute__((aligned(2)));
		short health,m_health,mp,max_mp		__attribute__((aligned(2)));
		unsigned char armor,skill		__attribute__((aligned(2)));
		short a[3]		__attribute__((aligned(2)));
		unsigned char a1_type,a23_type,m_type,speed,ap,mu,cl,breath,breath_type,treasure,spec_skill,poison		__attribute__((aligned(2)));
		short morale,m_morale		__attribute__((aligned(2)));
		short corpse_item,corpse_item_chance		__attribute__((aligned(2)));
		short status[15]		__attribute__((aligned(2)));
		unsigned char direction,immunities,x_width,y_width,radiate_1,radiate_2		__attribute__((aligned(2)));
		unsigned char default_attitude,summon_type,default_facial_pic,res1,res2,res3		__attribute__((aligned(2)));
		short picture_num		__attribute__((aligned(2)));
		
	};
	
	struct horse_record_type {
		location horse_loc,horse_loc_in_sec,horse_sector		__attribute__((aligned(2)));
		short which_town		__attribute__((aligned(2)));
		bool exists,property		__attribute__((aligned(2)));
	};
	
	struct boat_record_type {
		location boat_loc,boat_loc_in_sec,boat_sector		__attribute__((aligned(2)));
		short which_town		__attribute__((aligned(2)));
		bool exists,property		__attribute__((aligned(2)));
	};
	
	struct talk_save_type {
		short personality		__attribute__((aligned(2)));
		short town_num		__attribute__((aligned(2)));
		short str1,str2		__attribute__((aligned(2)));
	};
	
	struct creature_data_type {
		short active,attitude		__attribute__((aligned(2)));
		unsigned char number		__attribute__((aligned(2)));
		location m_loc		__attribute__((aligned(2)));
		monster_record_type m_d		__attribute__((aligned(2)));
		short mobile		__attribute__((aligned(2)));
		short summoned		__attribute__((aligned(2)));
		creature_start_type monst_start		__attribute__((aligned(2)));
	};
	
	struct creature_list_type {
		creature_data_type dudes[60]		__attribute__((aligned(2)));
		short which_town		__attribute__((aligned(2)));
		short friendly		__attribute__((aligned(2)));
	};
	
	struct outdoor_creature_type {
		bool exists		__attribute__((aligned(2)));
		short direction		__attribute__((aligned(2)));
		out_wandering_type what_monst		__attribute__((aligned(2)));
		location which_sector,m_loc		__attribute__((aligned(2)));	
	};
	
	struct party_record_type {
		long age		__attribute__((aligned(2)));
		short gold,food		__attribute__((aligned(2)));
		unsigned char stuff_done[310][10],item_taken[200][8]		__attribute__((aligned(2)));
		short light_level		__attribute__((aligned(2)));
		location outdoor_corner,i_w_c,p_loc,loc_in_sec		__attribute__((aligned(2)));
		boat_record_type boats[30]		__attribute__((aligned(2)));
		horse_record_type horses[30]		__attribute__((aligned(2)));
		creature_list_type creature_save[4]		__attribute__((aligned(2)));
		short in_boat,in_horse		__attribute__((aligned(2)));
		outdoor_creature_type out_c[10]		__attribute__((aligned(2)));
		item_record_type magic_store_items[5][10]		__attribute__((aligned(2)));
		short imprisoned_monst[4]		__attribute__((aligned(2)));
		char m_seen[256]		__attribute__((aligned(2)));
		char journal_str[50]		__attribute__((aligned(2)));
		short journal_day[50]		__attribute__((aligned(2)));
		short special_notes_str[140][2]		__attribute__((aligned(2)));
		talk_save_type talk_save[120]		__attribute__((aligned(2)));
		short direction,at_which_save_slot		__attribute__((aligned(2)));
		char alchemy[20]		__attribute__((aligned(2)));
		bool can_find_town[200]		__attribute__((aligned(2)));
		short key_times[100]		__attribute__((aligned(2)));
		short party_event_timers[30]		__attribute__((aligned(2)));
		short global_or_town[30]		__attribute__((aligned(2)));
		short node_to_call[30]		__attribute__((aligned(2)));
		char spec_items[50],help_received[120]		__attribute__((aligned(2)));
		short m_killed[200]		__attribute__((aligned(2)));
		long total_m_killed,total_dam_done,total_xp_gained,total_dam_taken		__attribute__((aligned(2)));
		char scen_name[256]		__attribute__((aligned(2)));
	};
	
	struct scenario_data_type {
		unsigned char out_width,out_height,difficulty,intro_pic,default_ground		__attribute__((aligned(2)));
		unsigned char town_size[200]		__attribute__((aligned(2)));
		unsigned char town_hidden[200]		__attribute__((aligned(2)));
		short flag_a		__attribute__((aligned(2)));
		short intro_mess_pic,intro_mess_len		__attribute__((aligned(2)));
		location where_start,out_sec_start,out_start		__attribute__((aligned(2)));
		short which_town_start		__attribute__((aligned(2)));
		short flag_b		__attribute__((aligned(2)));
		short town_data_size[200][5]		__attribute__((aligned(2)));
		short town_to_add_to[10]		__attribute__((aligned(2)));
		short flag_to_add_to_town[10][2]		__attribute__((aligned(2)));
		short flag_c		__attribute__((aligned(2)));
		short out_data_size[100][2]		__attribute__((aligned(2)));
		Rect store_item_rects[3]		__attribute__((aligned(2)));
		short store_item_towns[3]		__attribute__((aligned(2)));
		short flag_e		__attribute__((aligned(2)));
		short special_items[50]		__attribute__((aligned(2)));
		short special_item_special[50]		__attribute__((aligned(2)));
		short rating,uses_custom_graphics		__attribute__((aligned(2)));
		short flag_f		__attribute__((aligned(2)));
		monster_record_type scen_monsters[256]		__attribute__((aligned(2)));
		boat_record_type scen_boats[30]		__attribute__((aligned(2)));
		horse_record_type scen_horses[30]		__attribute__((aligned(2)));
		short flag_g		__attribute__((aligned(2)));
		terrain_type_type ter_types[256]		__attribute__((aligned(2)));
		short scenario_timer_times[20]		__attribute__((aligned(2)));
		short scenario_timer_specs[20]		__attribute__((aligned(2)));
		short flag_h		__attribute__((aligned(2)));
		special_node_type scen_specials[256]		__attribute__((aligned(2)));
		item_storage_shortcut_type storage_shortcuts[10]		__attribute__((aligned(2)));
		short flag_d		__attribute__((aligned(2)));
		unsigned char scen_str_len[300]		__attribute__((aligned(2)));
		short flag_i		__attribute__((aligned(2)));
		location last_out_edited		__attribute__((aligned(2)));
		short last_town_edited		__attribute__((aligned(2)));
	};
	
	struct setup_save_type {
		unsigned char setup[4][64][64]		__attribute__((aligned(2)));
	};
	
	struct pc_record_type {
		short main_status		__attribute__((aligned(2)));
		char name[20]		__attribute__((aligned(2)));
		short skills[30]		__attribute__((aligned(2)));
		short max_health,cur_health,max_sp,cur_sp,experience,skill_pts,level		__attribute__((aligned(2)));
		short status[15]		__attribute__((aligned(2)));
		item_record_type items[24]		__attribute__((aligned(2)));
		bool equip[24]		__attribute__((aligned(2)));
		bool priest_spells[62],mage_spells[62]		__attribute__((aligned(2)));
		short which_graphic,weap_poisoned		__attribute__((aligned(2)));
		bool advan[15],traits[15]		__attribute__((aligned(2)));
		short race,exp_adj,direction		__attribute__((aligned(2)));
	};
	
	struct town_item_list {
		item_record_type items[115]		__attribute__((aligned(2)));
	};
	
	struct stored_town_maps_type {
		char town_maps[200][8][64]		__attribute__((aligned(2)));
	}; 
	
	typedef struct stored_outdoor_maps_type {
		char outdoor_maps[100][6][48]		__attribute__((aligned(2)));
	}; 
	
	struct stored_items_list_type {
		item_record_type items[115]		__attribute__((aligned(2)));
	};
	
	struct current_town_type {
		short town_num, difficulty		__attribute__((aligned(2)));
		town_record_type	town		__attribute__((aligned(2)));
		char explored[64][64]		__attribute__((aligned(2)));
		bool	hostile		__attribute__((aligned(2)));
		creature_list_type	monst		__attribute__((aligned(2)));
		bool	in_boat		__attribute__((aligned(2)));
		location p_loc		__attribute__((aligned(2)));
	};
	
	struct out_info_type {
		char expl[96][96]		__attribute__((aligned(2)));
	};
	
//	struct piles_of_stuff_dumping_type {
//		char strings_ls[NLS][40]		__attribute__((aligned(2)));
//		char strings_rs[NRS][40]		__attribute__((aligned(2)));
//		char town_strs[180][256]		__attribute__((aligned(2)));
//		char out_strs[120][256]		__attribute__((aligned(2)));
//		char scen_strs[270][256]		__attribute__((aligned(2)));
//		char talk_strs[170][256]		__attribute__((aligned(2)));
//		scen_item_data_type scen_item_list		__attribute__((aligned(2)));
//	};
};

#endif
