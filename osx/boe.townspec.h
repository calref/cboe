
#include <string>

void activate_monster_enc(short enc_num,std::string list,short str,short strsnd,short *flip_bit);
bool special_trap(short trap_type,short dialog_num,short *flip_bit);
bool run_trap(short pc_num,eTrapType trap_type,short trap_level,short diff);

location get_spec_loc(short which);
//void start_split(short a,short b,short noise) ;
//void end_split(short noise) ;
void switch_lever(location w);
short handle_lever(location w);
