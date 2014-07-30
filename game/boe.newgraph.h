#ifndef _NEWGRAPH_H
	#define _NEWGRAPH_H

typedef	struct {
	char word[15];
	RECT word_rect;
	} word_rect_type;

void apply_unseen_mask();
void apply_light_mask();

void start_missile_anim();
short get_missile_direction(POINT origin_point,POINT the_point);
void end_missile_anim() ;
void run_a_missile(location from,location fire_to,short miss_type,short path,short sound_num,short x_adj,short y_adj,short len);
void run_a_boom(location boom_where,short type,short x_adj,short y_adj);
void mondo_boom(location l,short type);
void add_missile(location dest,short missile_type,short path_type,short x_adj,short y_adj);
void add_explosion(location dest,short val_to_place,short place_type,short boom_type,short x_adj,short y_adj);
void do_missile_anim(short num_steps,location missile_origin,short sound_num) ;
void do_explosion_anim(short sound_num,short expand);
void click_shop_rect(RECT area_rect);
void draw_shop_graphics(short draw_mode,RECT clip_area_rect);
void refresh_shopping();
item_record_type store_mage_spells(short which_s) ;
item_record_type store_priest_spells(short which_s);
item_record_type store_alchemy(short which_s);
void get_item_interesting_string(item_record_type item,char *message);
void click_talk_rect(char const *str_to_place,char const *str_to_place2,RECT c_rect);
void place_talk_str(char const *str_to_place,char const *str_to_place2,short color,RECT c_rect);
short scan_for_response(char *str);
void refresh_talking();

#endif
