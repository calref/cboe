
#ifndef BOE_GAME_PARTY_H
#define BOE_GAME_PARTY_H

#include "spell.hpp"

class cDialog;
void make_boats();
bool create_pc(short spot,cDialog* parent_num);
bool take_sp(short pc_num,short amt);
void increase_light(short amt);
void award_party_xp(short amt);
void award_xp(short pc_num,short amt,bool force = false);
void drain_pc(cPlayer& who,short how_much); // TODO: Move to a member function
short check_party_stat(eSkill which_stat, short mode);
bool poison_weapon( short pc_num, short how_much,bool safe);
bool is_poisonable_weap(class cItem& weap);
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

// Reasons the player can't cast any Mage or Priest spells
enum eCastStatus {
	CAST_OK,
	// Immutable reasons, shouldn't be printed when multiple PCs are checked (peace mode)
	NO_CAST_SKILL,
	NO_CAST_ANAMA,
	// Should only be printed once when multiple PCs are checked (peace mode)
	NO_CAST_ANTIMAGIC,
	// Should be printed for each character when multiple PCs are checked (peace mode)
	NO_CAST_SP,
	NO_CAST_ENCUMBERED,
	NO_CAST_DUMBFOUNDED,
	NO_CAST_PARALYZED,
	NO_CAST_ASLEEP,
	// Fallthrough: If there are any ways this can be reached, they should be given their own enum values!
	NO_CAST_UNKNOWN,
	// Pacifism is intentionally not included here because it applies only to specific spells,
	// and disables them in the spell picker or when you use the item.
};
eCastStatus pc_can_cast_spell(const cPlayer& pc,const eSkill type);
bool pc_can_cast_spell(const cPlayer& pc,eSpell spell_num);
void print_cast_status(eCastStatus status, eSkill type, std::string pc_name = "");
eSpell pick_spell(short pc_num, eSkill type, bool check_done = false);

void start_town_targeting(eSpell s_num,short who_c,bool freebie,eSpellPat pat = PAT_SINGLE);
void do_alchemy();
eAlchemy alch_choice(short pc_num);
bool pick_pc_graphic(short pc_num,short mode,cDialog* parent_num);
bool pick_pc_name(short pc_num,cDialog* parent);
bool has_trapped_monst();
mon_num_t pick_trapped_monst();
bool flying() ;
void hit_party(short how_much,eDamageType damage_type,short snd_type = 0);
void slay_party(eMainStatus mode);
short damage_pc(cPlayer& which_pc,short how_much,eDamageType damage_type,eRace type_of_attacker, short sound_type = -1,bool do_print = true, bool boom = true);
void petrify_pc(cPlayer& which_pc,int strength);
void kill_pc(cPlayer& which_pc,eMainStatus type);
void set_pc_moves();
void take_ap(short num);
short trait_present(eTrait which_trait);
short race_present(eRace which_race);
short wilderness_lore_present(ter_num_t ter);
void print_spell_cast(eSpell spell,eSkill which);
void put_party_in_scen(std::string scen_name, bool force = false, bool allow_unpacked = false);
short party_size(bool only_living);
bool all_items_identified();

// This is defined in pc.editors.cpp since it is also used by the character editor
bool spend_xp(short pc_num, short mode, cDialog* parent);

#endif
