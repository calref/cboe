
#ifndef BOE_GAME_PARTY_H
#define BOE_GAME_PARTY_H

#include "spell.hpp"

class cDialog;
void make_boats();
bool create_pc(short spot,cDialog* parent_num);
bool take_sp(short pc_num,short amt);
void heal_pc(short pc_num,short amt);
void heal_party(short amt);
void cure_pc(short pc_num,short amt);
void cure_party(short amt);
void curse_pc(short which_pc,short how_much);
void dumbfound_pc(short which_pc,short how_much);
void disease_pc(short which_pc,short how_much);
void sleep_pc(short which_pc,short how_much,eStatus what_type,short adjust);
void slow_pc(short which_pc,short how_much);
void web_pc(short which_pc,short how_much);
void increase_light(short amt);
void restore_sp_pc(short pc_num,short amt);
void restore_sp_party(short amt);
void award_party_xp(short amt);
void award_xp(short pc_num,short amt);
void drain_pc(short which_pc,short how_much);
short check_party_stat(eSkill which_stat, short mode);
bool poison_weapon( short pc_num, short how_much,short safe);
bool is_weapon(short pc_num,short item);
void cast_spell(eSkill type);
bool repeat_cast_ok(eSkill type);
void give_party_spell(short which);
void do_mage_spell(short pc_num,eSpell spell_num, bool freebie = false);
void do_priest_spell(short pc_num,eSpell spell_num, bool freebie = false);
void cast_town_spell(location where);
bool cast_spell_on_space(location where, eSpell spell);
void crumble_wall(location where);
void do_mindduel(short pc_num,cCreature *monst);
void dispel_fields(short i,short j,short mode);
bool pc_can_cast_spell(short pc_num,eSpell spell_num);
bool pc_can_cast_spell(short pc_num,eSkill spell_num);
eSpell pick_spell(short pc_num,eSkill type);
short stat_adj(short pc_num,eSkill which);
void start_town_targeting(eSpell s_num,short who_c,bool freebie,eSpellPat pat = PAT_SINGLE);
void do_alchemy();
eAlchemy alch_choice(short pc_num);
bool pick_pc_graphic(short pc_num,short mode,cDialog* parent_num);
bool pick_pc_name(short pc_num,cDialog* parent)  ;
mon_num_t pick_trapped_monst();
bool flying() ;
void acid_pc(short which_pc,short how_much);
void poison_pc(short which_pc,short how_much);
void poison_party(short how_much);
void void_sanctuary(short pc_num);
void hit_party(short how_much,eDamageType damage_type,short snd_type = 0);
void slay_party(eMainStatus mode);
bool damage_pc(short which_pc,short how_much,eDamageType damage_type,eRace type_of_attacker, short sound_type,bool do_print = true);
void kill_pc(short which_pc,eMainStatus type);
void petrify_pc(short which_pc, short strength);
void set_pc_moves();
void take_ap(short num);
short trait_present(eTrait which_trait);
short race_present(eRace which_race);
short wilderness_lore_present();
void print_spell_cast(eSpell spell,eSkill which);
void put_party_in_scen(std::string scen_name);
short party_size(bool only_living);

// This is defined in pc.editors.cpp since it is also used by the character editor
bool spend_xp(short pc_num, short mode, cDialog* parent);

#endif
