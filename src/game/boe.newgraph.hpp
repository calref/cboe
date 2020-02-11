
#include <string>
#include <SFML/Graphics/RenderTarget.hpp>
#include "location.hpp"
#include "item.hpp"
#include "gfxsheets.hpp"

struct word_rect_t {
	std::string word;
	rectangle rect;
	int node = -1;
	sf::Color on, off;
	word_rect_t(std::string w, rectangle r) : word(w), rect(r) {}
	word_rect_t() {}
};

enum {
	TALK_DUNNO = -1,
	TALK_BUY = -2,
	TALK_SELL = -3,
	TALK_BUSINESS = -4,
	TALK_LOOK = -10,
	TALK_NAME = -11,
	TALK_JOB = -12,
	TALK_RECORD = -13,
	TALK_DONE = -14,
	TALK_BACK = -15,
	TALK_ASK = -16,
};

void apply_unseen_mask();
void apply_light_mask(bool onWindow);
void end_anim();
void init_anim(short which_anim);
void run_anim();
void run_dedicated_anim(short mode);
void run_anim_0();
void run_anim_1();
void run_anim_2();
void kill_a_monst(short which_m);
location anim_get_space(short which);
void put_anim_onscreen();
void set_anim_str(char *astr,location l);
void run_anim_pcs();
void set_anim_attacker(short attacker);
bool try_monst_anim_move(short i,location start,short x,short y);
void run_anim_monsts();
bool try_pc_anim_move(short i,location start,short x,short y);
void start_missile_anim();
short get_missile_direction(location origin_point,location the_point);
void end_missile_anim() ;
void run_a_missile(location from,location fire_to,miss_num_t miss_type,short path,short sound_num,short x_adj,short y_adj,short len);
void run_a_boom(location boom_where,short type,short x_adj,short y_adj,short snd = -1);
void mondo_boom(location l,short type,short snd = -1);
void add_missile(location dest,miss_num_t missile_type,short path_type,short x_adj,short y_adj);
void add_explosion(location dest,short val_to_place,short place_type,short boom_type,short x_adj,short y_adj);
void do_missile_anim(short num_steps,location missile_origin,short sound_num) ;
void do_explosion_anim(short sound_num,short expand,short snd = -1);
void click_shop_rect(rectangle area_rect);
void draw_shop_graphics(bool pressed,rectangle clip_area_rect);
void refresh_shopping();
std::string get_item_interesting_string(cItem item);
void click_talk_rect(word_rect_t word);
void place_talk_str(std::string str_to_place,std::string str_to_place2,short color,rectangle c_rect);
short scan_for_response(const char *str);
void refresh_talking();
graf_pos calc_item_rect(int num,rectangle& to_rect);
