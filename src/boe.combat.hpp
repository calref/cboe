
#ifndef BOE_GAME_COMBAT_H
#define BOE_GAME_COMBAT_H

#include "location.hpp"
#include "monster.hpp"
#include "outdoors.hpp"
#include "boe.global.hpp"
#include "spell.hpp"

void start_outdoor_combat(cOutdoors::cCreature encounter,location where,short num_walls);
bool pc_combat_move(location destination);
void char_parry();
void char_stand_ready();
void pc_attack(short who_att,iLiving* target);
void pc_attack_weapon(short who_att,iLiving& target,short hit_adj,short dam_adj,const cItem& weap,short primary,bool do_poison);
short calc_spec_dam(eItemAbil abil,short abil_str,short abil_dat,iLiving& monst,eDamageType& dmg_type);
void place_target(location target);
void do_combat_cast(location target);
void handle_marked_damage();
void load_missile();
void fire_missile(location target);
bool combat_next_step();
bool pick_next_pc();
void combat_run_monst();
void do_monster_turn();
void monster_attack(short who_att,iLiving* target);
void monst_fire_missile(short m_num,short bless,std::pair<eMonstAbil,uAbility> abil,location source,iLiving* target);
void monst_basic_abil(short m_num, std::pair<eMonstAbil,uAbility> abil, iLiving* target);
bool monst_breathe(cCreature *caster,location targ_space,uAbility dam_type);
bool monst_cast_mage(cCreature *caster,short targ);
bool monst_cast_priest(cCreature *caster,short targ);
void damage_target(short target,short dam,eDamageType type,short sound_type = 0);
location find_fireball_loc(location where,short radius,short mode,short *m);
location closest_pc_loc(location where);
short count_levels(location where,short radius);
bool pc_near(short pc_num,location where,short radius);
bool monst_near(short m_num,location where,short radius,short active);
void fireball_space(location loc,short dam);
void place_spell_pattern(effect_pat_type pat,location center,short who_hit);
void place_spell_pattern(effect_pat_type pat,location center,eFieldType type,short who_hit);
void place_spell_pattern(effect_pat_type pat,location center,eDamageType type, short dice,short who_hit);
void modify_pattern(effect_pat_type *pat,unsigned short type);
void do_shockwave(location target);
void radius_damage(location target,short radius, short dam, eDamageType type);
void hit_pcs_in_space(location target,short dam,eDamageType type,short report,short hit_all);
void hit_space(location target,short dam,eDamageType type,short report,short hit_all);
void do_poison();
void handle_disease();
void handle_acid();
bool no_pcs_left();
bool hit_end_c_button();
bool out_monst_all_dead();
void end_combat();
bool combat_cast_mage_spell();
bool combat_cast_priest_spell();
void combat_immed_mage_cast(short current_pc, eSpell spell_num, bool freebie = false);
void combat_immed_priest_cast(short current_pc, eSpell spell_num, bool freebie = false);
void start_spell_targeting(eSpell num, bool freebie = false, int spell_range = 4, eSpellPat pat = PAT_SINGLE);
void start_fancy_spell_targeting(eSpell num, bool freebie = false, int spell_range = 4, eSpellPat pat = PAT_SINGLE, int targets = 1);
void spell_cast_hit_return();
void process_fields();
void process_force_cage(location loc, short i, short bonus = 0);
void break_force_cage(location loc);
void scloud_space(short m,short n);
void web_space(short m,short n);
void sleep_cloud_space(short m,short n);
void take_m_ap(short num,cCreature *monst);
void add_new_action(short pc_num);
short get_monst_sound(cCreature *attacker,short which_att);

#endif
