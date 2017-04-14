
#ifndef BOE_GAME_GRAPHUTIL_H
#define BOE_GAME_GRAPHUTIL_H

#include "pict.hpp"
#include "location.hpp"
#include "gfxsheets.hpp"

void draw_one_terrain_spot (short i,short j,short terrain_to_draw);
void draw_monsters();
void play_see_monster_str(unsigned short m, location monst_loc);
void draw_combat_pc(class cPlayer& who, location center, bool attacking);
void frame_active_pc(location center);
void draw_pcs(location center);
void draw_outd_boats(location center);
void draw_town_boat(location center) ;
void draw_party_symbol(location center);
rectangle get_monster_template_rect (pic_num_t type_wanted,short mode,short which_part) ;
bool is_fluid(ter_num_t ter_type);
bool is_shore(ter_num_t ter_type);
bool is_wall(ter_num_t ter_type);
bool is_ground(ter_num_t ter_type);
char get_fluid_trim(location where,ter_num_t ter_type);
void check_if_monst_seen(unsigned short m_num, location monst_loc);
void play_ambient_sound();
graf_pos calc_item_rect(int num,rectangle& to_rect);

void draw_items(location where);
void draw_fields(location where);

#endif
