/*
 *  pc.h
 *  BoE
 *
 *  Created by Celtic Minstrel on 24/04/09.
 *
 */


namespace legacy { struct pc_record_type; };

class cPlayer {
public:
	short main_status;
	char name[20];
	short skills[30];
	short max_health;
	short cur_health;
	short max_sp;
	short cur_sp;
	unsigned short experience;
	short skill_pts;
	short level;
	short status[15];
	cItemRec items[24];
	bool equip[24];
	bool priest_spells[62];
	bool mage_spells[62];
	short which_graphic;
	short weap_poisoned;
	bool advan[15];
	bool traits[15];
	short race;
	short exp_adj;
	short direction;
	
	cPlayer& operator = (legacy::pc_record_type old);
	cPlayer();
	cPlayer(long key,short slot);
	short get_tnl();
};
