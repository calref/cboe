/*
 *  pc.h
 *  BoE
 *
 *  Created by Celtic Minstrel on 24/04/09.
 *
 */


namespace legacy { struct pc_record_type; };

enum eMainStatus {
	MAIN_STATUS_ABSENT = 0, // absent, empty slot
	MAIN_STATUS_ALIVE = 1,
	MAIN_STATUS_DEAD = 2,
	MAIN_STATUS_DUST = 3,
	MAIN_STATUS_STONE = 4,
	MAIN_STATUS_FLED = 5,
	MAIN_STATUS_SURFACE = 6, // fled to surface?
	MAIN_STATUS_WON = 7,
	MAIN_STATUS_SPLIT = 10,
	// The rest are not really necessary, but are here for completeness so that all valid values have a name.
	MAIN_STATUS_SPLIT_ABSENT = MAIN_STATUS_SPLIT + MAIN_STATUS_ABSENT,
	MAIN_STATUS_SPLIT_ALIVE = MAIN_STATUS_SPLIT + MAIN_STATUS_ALIVE,
	MAIN_STATUS_SPLIT_DEAD = MAIN_STATUS_SPLIT + MAIN_STATUS_DEAD,
	MAIN_STATUS_SPLIT_DUST = MAIN_STATUS_SPLIT + MAIN_STATUS_DUST,
	MAIN_STATUS_SPLIT_STONE = MAIN_STATUS_SPLIT + MAIN_STATUS_STONE,
	MAIN_STATUS_SPLIT_FLED = MAIN_STATUS_SPLIT + MAIN_STATUS_FLED,
	MAIN_STATUS_SPLIT_SURFACE = MAIN_STATUS_SPLIT + MAIN_STATUS_SURFACE,
	MAIN_STATUS_SPLIT_WON = MAIN_STATUS_SPLIT + MAIN_STATUS_WON,
};

class cPlayer {
public:
	eMainStatus main_status;
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

void operator += (eMainStatus& stat, eMainStatus othr);
void operator -= (eMainStatus& stat, eMainStatus othr);
