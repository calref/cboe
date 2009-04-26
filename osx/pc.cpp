/*
 *  pc.cpp
 *  BoE
 *
 *  Created by Celtic Minstrel on 24/04/09.
 *
 */

#include "classes.h"
#include "oldstructs.h"

__attribute__((deprecated))
cPlayer& cPlayer::operator = (legacy::pc_record_type old){
	int i;
	main_status = (eMainStatus) old.main_status;
	strcpy(name,old.name);
	for(i = 0; i < 20; i++)
		skills[i] = old.skills[i];
	max_health = old.max_health;
	cur_health = old.cur_health;
	max_sp = old.max_sp;
	cur_sp = old.cur_sp;
	experience = old.experience;
	skill_pts = old.skill_pts;
	level = old.level;
	for(i = 0; i < 15; i++){
		status[i] = old.status[i];
		advan[i] = old.advan[i];
		traits[i] = old.traits[i];
	}
	for(i = 0; i < 24; i++){
		items[i] = old.items[i];
		equip[i] = old.equip[i];
	}
	for(i = 0; i < 62; i++){
		priest_spells[i] = old.priest_spells[i];
		mage_spells[i] = old.mage_spells[i];
	}
	which_graphic = old.which_graphic;
	weap_poisoned = old.weap_poisoned;
	race = old.race;
	exp_adj = old.exp_adj;
	direction = old.direction;
}

short cPlayer::get_tnl(){
	short tnl = 100,i,store_per = 100;
	static const short rp[3] = {0,12,20};
	static const short ap[15] = {10,20,8,10,4, 6,10,7,12,15, -10,-8,-8,-20,-8};
	
	tnl = (tnl * (100 + rp[race])) / 100;
	for (i = 0; i < 15; i++)
		if (traits[i]) 
			store_per = store_per + ap[i];
	
	tnl = (tnl * store_per) / 100;	
	
	return tnl;
}

cPlayer::cPlayer(){
	short i;
	main_status = MAIN_STATUS_ABSENT;
	sprintf	((char *) name, "\n");
	
	for (i = 0; i < 30; i++)
		skills[i] = (i < 3) ? 1 : 0;
	cur_health = 6; 
	max_health = 6; 
 	cur_sp = 0; 
 	max_sp = 0; 
 	experience = 0; 
	skill_pts = 60; 
	level = 1;
 	for (i = 0; i < 15; i++)
 		status[i] = 0;
	for (i = 0; i < 24; i++)
		items[i] = cItemRec();
	for (i = 0; i < 24; i++)
		equip[i] = FALSE;
	
 	for (i = 0; i < 62; i++) {
		priest_spells[i] = (i < 30) ? TRUE : FALSE;
		mage_spells[i] = (i < 30) ? TRUE : FALSE;
	}
	which_graphic = 0;
	weap_poisoned = 24;
	
	for (i = 0; i < 15; i++) {
		advan[i] = FALSE;
		traits[i] = FALSE;	
	}		
	race = 0;
	exp_adj = 100;
	direction = 0;
}

cPlayer::cPlayer(long key,short slot){
	short i;
	main_status = MAIN_STATUS_ALIVE;
	if(key == 'dbug'){
		switch (slot) {
			case 0:
				sprintf	((char *) name, "Gunther");
				break;
			case 1:
				sprintf	((char *) name, "Yanni");
				break;
			case 2:
				sprintf	((char *) name, "Mandolin");
				break;
			case 3:
				sprintf	((char *) name, "Pete");
				break;
			case 4:
				sprintf	((char *) name, "Vraiment");
				break;
			case 5:
				sprintf	((char *) name, "Goo");
				break;
		}
		for (i = 0; i < 30; i++)
			skills[i] = (i < 3) ? 20 : 8;
		cur_health = 60; 
		max_health = 60; 
		cur_sp = 90; 
		max_sp = 90; 
		experience = 0; 
		skill_pts = 60; 
		level = 1;
		for (i = 0; i < 15; i++)
			status[i] = 0;
		for (i = 0; i < 24; i++)
			items[i] = cItemRec();
		for (i = 0; i < 24; i++)
			equip[i] = FALSE;
		
		for (i = 0; i < 62; i++) {
			priest_spells[i] = TRUE;
			mage_spells[i] = TRUE;
		}
		//which_graphic = num * 3 + 1;	// 1, 4, 7, 10, 13, 16
		which_graphic = slot + 4;		// 4, 5, 6, 7,  8,  9
		weap_poisoned = 24; // was 16, as an E2 relic
		
		for (i = 0; i < 15; i++) {
			advan[i] = FALSE;
			traits[i] = FALSE;	
		}		
		
		race = 0;
		exp_adj = 100;
		direction = 0;
	}else if(key == 'dflt'){
		static const short pc_stats[6][19] = {
			{8,6,2,  6,0,0,0,0,0,  0,0,0,0,1,  0,0,2,0,0},
			{8,7,2,  0,0,6,3,0,3,  0,0,0,0,0,  0,0,0,2,0},
			{8,6,2,  3,3,0,0,2,0,  0,0,0,0,0,  4,4,0,2,1},
			{3,2,6,  2,0,0,2,0,0,  3,0,3,0,1,  0,0,0,0,0},
			{2,2,6,  3,0,0,2,0,0,  2,1,4,0,0,  0,0,0,0,1},
			{2,2,6,  0,2,0,2,0,1,  0,3,3,2,0,  0,0,0,0,0}
		};
		static const short pc_health[6] = {22,24,24,16,16,18};
		static const short pc_sp[6] = {0,0,0,20,20,21};
		static const short pc_graphics[6] = {3,32,29,16,23,14};
		static const short pc_race[6] = {0,2,1,0,0,0};
		static const short pc_t[6][15] = {
			{0,0,1,0,0, 0,1,0,0,0, 0,1,0,0,0},		
			{1,0,0,0,0, 1,0,0,0,0, 1,0,0,0,0},	
			{0,0,0,1,0, 0,0,0,0,0, 0,0,1,0,0},	
			{0,1,0,0,0, 0,0,0,0,0, 0,0,0,0,0},	
			{0,0,0,0,1, 0,1,1,0,0, 0,0,0,0,1},	
			{0,1,0,0,0, 0,0,0,0,0, 0,0,0,0,0}
		};
		
		main_status = MAIN_STATUS_ALIVE;
		switch (slot) {
			case 0:
				sprintf	((char *) name, "Jenneke");
				break;
			case 1:
				sprintf	((char *) name, "Thissa");
				break;
			case 2:
				sprintf	((char *) name, "Frrrrrr");
				break;
			case 3:
				sprintf	((char *) name, "Adrianna");
				break;
			case 4:
				sprintf	((char *) name, "Feodoric");
				break;
			case 5:
				sprintf	((char *) name, "Michael");
				break;
				
		}
		
		for (i = 0; i < 19; i++)
			skills[i] = pc_stats[slot][i];
		cur_health = pc_health[slot]; 
		max_health = pc_health[slot]; 
		experience = 0; 
		skill_pts = 0; 
		level = 1;
		
		for (i = 0; i < 15; i++)
			status[i] = 0;
		for (i = 0; i < 24; i++)
			items[i] = cItemRec();
		for (i = 0; i < 24; i++)
			equip[i] = FALSE;
		cur_sp = pc_sp[slot]; 
		max_sp = pc_sp[slot]; 
		for (i = 0; i < 62; i++) {
			priest_spells[i] = (i < 30) ? TRUE : FALSE;
			mage_spells[i] = (i < 30) ? TRUE : FALSE;
		}
		for (i = 0; i < 15; i++) {
			traits[i] = pc_t[slot][i];
			advan[i] = FALSE;
		}
		
		race = pc_race[slot];
		exp_adj = 100;
		direction = 0;
		
		which_graphic = pc_graphics[slot];
	}
}

void operator += (eMainStatus& stat, eMainStatus othr){
	if(othr == MAIN_STATUS_SPLIT)
		stat = (eMainStatus) (10 + stat);
}

void operator -= (eMainStatus& stat, eMainStatus othr){
	if(othr == MAIN_STATUS_SPLIT)
		stat = (eMainStatus) (-10 + stat);
}

//