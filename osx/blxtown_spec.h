void activate_monster_enc(short enc_num,short str1,short str2,short strsnd,short *flip_bit);
Boolean special_trap(short trap_type,short dialog_num,short *flip_bit);
void OTS(short str1b,short str2b,short str12a,short sound,unsigned char *flip_bit);
void DSG(short item_num,unsigned char *flip_bit,short dialog_num,short what_spec,short amt_gold,short amt_food);
Boolean GFI(short dialog_num);
Boolean run_trap(short pc_num,short trap_type,short trap_level,short diff);

location get_spec_loc(short which);
void start_split(short a,short b,short noise) ;
void end_split(short noise) ;
void switch_lever(location w);
short handle_lever(location w);
