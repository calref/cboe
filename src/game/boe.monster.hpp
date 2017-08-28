
#include "pict.hpp"
#include "creature.hpp"

short out_enc_lev_tot(short which);
void create_wand_monst();
void place_outd_wand_monst(location where,cOutdoors::cWandering group,short forced);
location get_monst_head(short m_num);
short get_monst_picnum(mon_num_t monst);
ePicType get_monst_pictype(mon_num_t monst);
std::pair<short,short> get_monst_dims(mon_num_t monst);
void set_up_monst(eAttitude mode,mon_num_t m_num);
void do_monsters();
bool monst_hate_spot(short which_m,location *good_loc);
short monst_pick_target(short which_m);
short monst_pick_target_monst(cCreature *which_m);
short monst_pick_target_pc(short m_num,cCreature *which_m);
short select_active_pc();
short closest_pc(location where);
short closest_monst(location where,bool friendly);
short switch_target_to_adjacent(short which_m,short orig_target);
bool rand_move(mon_num_t i);
bool seek_party(short i,location l1,location l2);
bool flee_party(short i,location l1,location l2);
bool try_move(short i,location start,short x,short y);
bool combat_move_monster(short which,location destination);
location find_clear_spot(location from_where,short mode);
location random_shift(location start);
bool outdoor_move_monster(short num,location dest);
bool town_move_monster(short num,location dest);
bool monster_placid(short m_num);
void monst_inflict_fields(short which_monst);
bool monst_check_special_terrain(location where_check,short mode,short which_monst);
void record_monst(cCreature* which_m, bool forced=false);
short place_monster(mon_num_t which,location where,bool forced=false);
bool summon_monster(mon_num_t which,location where,short duration,eAttitude given_attitude,bool by_party);
void activate_monsters(short code,short attitude);
short get_encumbrance(short pc_num);
mon_num_t get_summon_monster(short summon_class);
